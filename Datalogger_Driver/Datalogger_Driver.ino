#include "Adafruit_BME280.h" //Temp, pressure, humidity sensor
#include "Adafruit_Sensor.h" //Universal sensor API
#include "Adafruit_TCS34725.h" //Color Sensor
#include "Adafruit_TSL2591.h" //Light Sensor
#include "LiquidCrystalFast.h" //Display
#include <Wire.h> //I2C
#include <TimeLib.h> //Set RTC time and get time strings
#include <Snooze.h> //Put Teensy into low power state between log points
#include <SD.h> //Store data onto SD card

//Setup default parameters
uint16_t n_logs_per_file = 10000; //The number of logs to save to a file before creating a new low file
int log_interval[] = {0, 0, 1}; //Number of hours, minutes, and seconds between log intervals
const char boot_dir[] = "boot_log"; //Directory to save boot log files into - max length 8 char
const char log_dir[] = "data_log"; //Directory to save data log files into - max length 8 char
boolean measure_temp = true;
boolean measure_humidity = true;
boolean measure_pressure = true;
boolean measure_lux = true;
boolean measure_color = true;
boolean measure_battery = true;

boolean disable_display_on_log = false; //Completely power down display during logging
const float default_backlight = 1.0; //Set default backlight intensity to full brightness (range is 0-1)
const float default_contrast = 0.5; //Set default LCD contrast to half range (range is 0-1)

//Allow Teensy to re-run setup when settings are changed - Call "CPU_RESTART"
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// Load drivers
SnoozeDigital digital;
SnoozeAlarm  alarm;
SnoozeBlock hibernate_config(digital, alarm);

//Setup LCD pin numbers and initial parameters
const uint8_t LCD_pin[] = {30, 34, 35, 5, 4, 3, 1};
              // LCD pins: RS  RW  EN  D4 D5 D6 D7
LiquidCrystalFast lcd(LCD_pin[0], LCD_pin[1], LCD_pin[2], LCD_pin[3], LCD_pin[4], LCD_pin[5], LCD_pin[6]); //Only 4-pin supported in LiquidCrystalFast
const uint8_t LCD_toggle_pin = 24; //Set to high to power on LCD
const uint8_t LED_PWM_pin = 29; //Drive LED backlight intensity
const uint8_t contrast_pin = A21; //DAC pin for addjusting diplay contrast
const uint8_t analog_resolution = 16; //Number of bits in PWM and DAC analog  - PWM cap is 16, DAC cap is 12 - auto capped in code - https://www.pjrc.com/teensy/td_pulse.html
const uint16_t analog_max = (1<<analog_resolution)-1; //Highest analog value
const uint32_t analog_freq = 24000000/(1<<analog_resolution); //Calculate freq based on fastest for minimum clock speed - 24 MHz

//Define LCD custom characters
const uint8_t up_arrow[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

const uint8_t down_arrow[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

//Setup sensor pin numbers
const uint8_t temp_power_pin = 20; //Set Vcc pins
const uint8_t color_power_pin = 23;
const uint8_t light_power_pin = 17;
const uint8_t color_interrupt_pin = 16; //Set interrupt pins
const uint8_t light_interrupt_pin = 33;
const uint8_t I2C_pullup_pin = 12; //Pin providing I2C pullup voltage
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Setup joystick pins
const uint8_t joystick_pins[] = {9, 11, 2, 7, 10}; //Joystick pins - up, right, down, left, push 

//Setup battery test pin numbers
const uint8_t coin_test_pin = 36;
const uint8_t coin_analog_pin = A1;
const uint8_t Vin_test_pin = 39;
const uint8_t Vin_analog_pin = A0; 

//Setup SD card
const int chipSelect = BUILTIN_SDCARD;
Sd2Card card;
SdVolume volume;
SdFile root;
boolean boot_file_saved = false; //Whether the boot log has been saved to the SD card

//Component status
boolean display_present = false;
boolean display_on = false;
boolean backlight_on = false;
boolean temp_present = false;
boolean temp_on = false;
boolean color_present = false;
boolean color_on = false;
boolean light_present = false;
boolean light_on = false;
boolean coin_present = false;
boolean SD_present = false;

//File status
const uint8_t boot_dim_y = 20; //Number of rows (total lines)
const uint8_t LCD_dim_x = 21; //Number of columns (characters per line) - add one character for null character to delineate strings
const uint8_t LCD_max_lines = 25; //Maximum number of lines per window
char boot_disp[LCD_max_lines][LCD_dim_x]; //Array for boot display
const uint8_t log_disp_dim_y = 14; //Number of rows (total lines)
char log_disp[LCD_max_lines][LCD_dim_x] = {"Log Status:         ","Date:               ","Time:               ","Temp(°C):           ","Pres(hPa):          ","Humidity(%):        ","lux:                ","Red(µW):            ","Green(µW):          ","Blue(µW):           ","Clear(uW):          ","Vin:                ","Vbat:               ","Comment:            ","                    ","                    ","                    ","                    ","                    ","                    ","                    ","                    ","                    ","                    ","                    "};
const uint8_t settings_dim_y = 23; //Number of rows (total lines)
char settings_disp[LCD_max_lines][LCD_dim_x] = {"Settings:           ","-------SENSORS------","Temperature:        ","Humidity:           ","Pressure:           ","Lux:                ","Color:              ","Battery:            ","----LOG INTERVAL----", "Seconds:            ","Minutes:            ","Hours:              ","----LCD SETTINGS----","Contrast:           ","Backlight:          ","Disable on log:     ","----LOG INTERVAL----", "Seconds:            ","Minutes:            ","Hours:              ","----RTC SETTINGS----","Date:               ","Time:               ","                    ","                    "};
const uint8_t n_windows = 3;
char *LCD_windows[] = {(char *) boot_disp, (char *) log_disp, (char *) settings_disp}; //Array of windows available for LCD
uint8_t LCD_window_index = 0; //Current LCD window being displayed
uint8_t LCD_line_index = 0; //Current index of top line being displayed
uint8_t LCD_window_lines[] = {boot_dim_y, log_disp_dim_y, settings_dim_y}; //Array of total number of lines for each window
const char log_header[] = {"Date,Time,Temperature(°C),Pressure(hPa),Humidity(%),Intensity(lux),Red(µW/cm^2),Green(µW/cm^2),Blue(µW/cm^2),Clear(µW/cm^2),Vin(V),Vbat(V),Comment,"};
uint8_t boot_index = 0; //Index of boot message
int warning_count = 0; //Number of warnings encountered during boot 
const uint16_t log_dim_y = 1000; //Number of rows (total lines)
const uint8_t log_dim_x = 100; //Number of columns (characters per line)
volatile char internal_log_backup[log_dim_y][log_dim_x]; //Array for storing data logs while SD card is not available
uint16_t log_internal_index = 0; //Index of internal log array
uint16_t log_file_index = 0; //Index of current line in log file
char current_file_name[13]; //Storing the current log file name - max length for FAT32 is 8.3 - 13 characters total including null at end
int log_file_count = 0; //Counter for tracking number of log files in ascii (track files from aa to zz)
boolean log_active = false; //Whether the device is actively logging or in standby state

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_60X); //Create instance of color sensor initialize with peak sensitivity
Adafruit_TSL2591 light_sensor = Adafruit_TSL2591(2591); //Create instance of light sensor - number is sensor ID


//------------------------------------------------------------------------------
// call back for file timestamps - from: https://forum.arduino.cc/index.php?topic=348562.0
void dateTime(uint16_t* date, uint16_t* time) {
 time_t unix_t = now();
 // return date using FAT_DATE macro to format fields
 *date = FAT_DATE(year(unix_t), month(unix_t), day(unix_t));
 // return time using FAT_TIME macro to format fields
 *time = FAT_TIME(hour(unix_t), minute(unix_t), second(unix_t));
}
//------------------------------------------------------------------------------

time_t unix_t = 0;
float contrast = 0;

void setup() {
  initializeDevice();
}

void loop() {
  uint8_t wakeup_source;
  //wakeup_source = Snooze.hibernate(hibernate_config);
  wakeupEvent(joystick_pins[0]);
}

void wakeupEvent(uint8_t src){
  //joystick_pins[] = {9, 11, 2, 7, 10}; //Joystick pins - up, right, down, left, push 
  if(src == joystick_pins[0] || src == joystick_pins[2]){
    scrollWindow(src);
  }
  else if(src == joystick_pins[1] || src == joystick_pins[3]){
    //cycleWindow();
  }
  else if(src == joystick_pins[4]){
    //centerPress();
  }
  else{
    //logDataPoint();
  }
  digitalWriteFast(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWriteFast(LED_BUILTIN, LOW);
  delay(1000); //If log is not active, ignore RTC alarm
}

//Use the up and down buttons to vertically scroll through the current window
void scrollWindow(uint8_t src){
  char disp_line[LCD_dim_x];
  char* disp_pointer = LCD_windows[LCD_window_index];
  if(src == joystick_pins[0]){ //If up press, scroll window up
    if(!LCD_line_index) return; //If already at the top, do nothing
    else LCD_line_index--;
  }
  else{ //If down press, scroll window down
    if(LCD_line_index == LCD_window_lines[LCD_window_index]-4) return; //If already at the bottom, do nothing
    else LCD_line_index++; 
  }

  for(int a=LCD_line_index; a<LCD_line_index+4; a++){
    for(int b=0; b<LCD_dim_x; b++){
      disp_line[b] = *((disp_pointer + a*LCD_dim_x) + b);
    }
    disp_line[LCD_dim_x-1] = 0; //Force termination at end of string - prevent overlfow issues
    Serial.println(disp_line);
    lcd.setCursor(0,a-LCD_line_index);
    lcd.println(disp_line);  
  }
}
//  n_windows
//  char *LCD_windows[3] = {(*boot_disp)[LCD_dim_x], (*log_disp)[LCD_dim_x], (*settings_disp)[LCD_dim_x]}; //Array of windows available for LCD
//uint8_t LCD_window_index = 0; //Current LCD window being displayed
//uint8_t LCD_line_index = 0; //Current index of top line being displayed
//uint8_t LCD_window_lines[] = {boot_dim_y, log_disp_dim_y, settings_dim_y}; //Array of total number of lines for each window

//Check all components and write boot log
void initializeDevice(){
  log_active = false;
  boot_index = 0;
  strcpy(boot_disp[boot_index++], "Boot status:        ");

  //Turn off Teensy LED
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWriteFast(LED_BUILTIN, LOW);

  //Initialize joystick pins - map to digital wake from hibernate
  for(int a = 0; a<5; a++){
    digital.pinMode(joystick_pins[a], INPUT_PULLUP, RISING);
  }

  //Initialize RTC timer wake from hibernate
  alarm.setRtcTimer(log_interval[0], log_interval[1], log_interval[2]);
  
  //Power on the sensors
  pinMode(temp_power_pin, OUTPUT);
  pinMode(color_power_pin, OUTPUT);
  pinMode(light_power_pin, OUTPUT);
  pinMode(I2C_pullup_pin, OUTPUT);
  digitalWriteFast(temp_power_pin, (measure_temp || measure_humidity || measure_pressure)); //Power BME280 if any evironmental variable is to be sensed
  digitalWriteFast(color_power_pin, measure_color);
  digitalWriteFast(light_power_pin, measure_lux);
  digitalWriteFast(I2C_pullup_pin, (measure_temp || measure_humidity || measure_pressure || measure_lux || measure_color));

  //Set battery test pins
  pinMode(coin_test_pin, OUTPUT);
  pinMode(Vin_test_pin, OUTPUT);
  digitalWriteFast(coin_test_pin, LOW); //Disconnect coin cell from ADC
  digitalWriteFast(Vin_test_pin, LOW); //Disconnect battery from ADC
  if(measure_battery){
    pinMode(coin_analog_pin, INPUT); //Set coin cell ADC to floating high impedance
    pinMode(Vin_analog_pin, INPUT); //Set battery ADC to floating high impedance
  }
  
  //Setup USB serial communication
  Serial.begin(9600); //Baud rate is ignored and negotiated with computer for max speed
  wakeUSB();

  //Synchronize Teensy clock to computer clock
  setSyncProvider(getTeensy3Time); //Sync to computer clock - do not use () in function as argument
  if (timeStatus()!= timeSet) { //If sync failed
    strcpy(boot_disp[boot_index++], "RTC time sync fail! ");
    warning_count += 1;
  } else { // if Sync successful
    strcpy(boot_disp[boot_index++], "RTC sync successful ");
  }
  unix_t = now();
  sprintf(boot_disp[boot_index++], "%4d/%02d/%02d %02d:%02d:%02d ", year(unix_t), month(unix_t), day(unix_t), hour(unix_t), minute(unix_t), second(unix_t)); //Write current time to boot screen
  
  //Setup I2C communication to highest speed chips sill support - sensor libraries will initialize I2C communications
  Wire.setClock(400000); 
  Wire1.setClock(400000);

  //Start LCD display
  analogWriteResolution(analog_resolution); //Set DAC and PWM resolution - NOTE: Do not change once set!
  analogReadResolution(analog_resolution); //Set ADC resolution - NOTE: Do not change once set!
  analogWriteFrequency(LED_PWM_pin, analog_freq); //Set LED PWM freq - other pins on same timer will also change - https://www.pjrc.com/teensy/td_pulse.html
  lcd.begin(20, 4); //Initialize LCD
  lcd.createChar(0, up_arrow); //Create arrow characters
  lcd.createChar(1, down_arrow);
  setLCDbacklight(default_backlight); //Turn on LED backlight to default intensity
  setLCDcontrast(default_contrast); //Initialize screen contrast to default value

  if(monitorPresent()){ //confirm if LCD is present
    strcpy(boot_disp[boot_index++], "Display initialized ");
    display_present = true;
    display_on = true;
    if(default_backlight > 0){
      backlight_on = true;
    }
    else{
      backlight_on = false;
    }
  }
  else{
    strcpy(boot_disp[boot_index++], "Display not found!  ");
    warning_count += 1;
    disableDisplay();
    display_present = false;
    display_on = false;
    backlight_on = false;
  }

  //Initialize sensors
  if(temp_sensor.begin(0x77, temp_port)){
    strcpy(boot_disp[boot_index++], "Temp sensor....OK   ");
    temp_present = true;
    temp_on = true;
  }
  else{
    strcpy(boot_disp[boot_index++], "Temp sensor....FAIL!");
    warning_count += 1;
    digitalWriteFast(temp_power_pin, LOW); //Cut power to sensor
    temp_present = false;
    temp_on = false;
  }
  if(color_sensor.begin(0x29, color_port)){
    strcpy(boot_disp[boot_index++], "Color sensor...OK  ");
    color_present = true;
    color_on = true;
  }
  else{
    strcpy(boot_disp[boot_index++], "Color sensor...FAIL!");
    warning_count += 1;
    digitalWriteFast(color_power_pin, LOW); //Cut power to sensor
    color_present = false;
    color_on = false;
  }
  if(light_sensor.begin(light_port)){
    strcpy(boot_disp[boot_index++], "Light sensor...OK   ");
    light_present = true;
    light_on = true;    
  }
  else{
    strcpy(boot_disp[boot_index++], "Light sensor...FAIL!");
    warning_count += 1;
    digitalWriteFast(light_power_pin, LOW); //Cut power to sensor
    light_present = false;
    light_on = false;  
  }
  digitalWriteFast(I2C_pullup_pin, LOW); //Power down I2C pullup as sensor communication is complete
  
  //Test batteries
  float volt = checkVbat();
  sprintf(boot_disp[boot_index++], "VBat: %4.2fV         ", volt);
  if(volt > 0){
    coin_present = true;
    if(volt < 2.8){
      strcpy(boot_disp[boot_index++], "Vbat low voltage!   ");
      warning_count += 1;
    }
  }

  volt = checkVin();
  sprintf(boot_disp[boot_index++], "Vin:  %4.2fV         ", volt);
  if(volt < 3.6){
    strcpy(boot_disp[boot_index++], "Vin low voltage!    ");
    warning_count += 1;
  }
  
  // set date time callback function for applying RTC synced time stamps to SD card file time stamps
  SdFile::dateTimeCallback(dateTime);

  //Test SD card
  if (card.init(SPI_HALF_SPEED, chipSelect)) { //Check if SD card is present
    strcpy(boot_disp[boot_index++], "SD card found       ");
  }
  else{
    strcpy(boot_disp[boot_index++], "SD card not found!  ");
    warning_count += 1;
  }
  
  if (volume.init(card)) { //Check that there is a FAT32 partition
    int partition = volume.fatType();
    float volumesize;
    sprintf(boot_disp[boot_index++], "FAT%2d volume found  ", partition);
    volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
    volumesize *= volume.clusterCount();       // we'll have a lot of clusters
    volumesize /= 2;
    if(volumesize >= 1e3){ //If volume is more than 1MB in size - report size as MB
      volumesize /= 1000;
      if(volumesize >= 1e3){ //If volume is more than 1GB in size - report size as GB
        volumesize /= 1000;
        if(volumesize >= 1e3){ //If volume is more than 1TB in size - report size as TB
          volumesize /= 1000;
          sprintf(boot_disp[boot_index++], "Size: % 8.4fTB    ", volumesize); 
        }
        else sprintf(boot_disp[boot_index++], "Size: % 8.4fGB    ", volumesize); 
      }
      else sprintf(boot_disp[boot_index++], "Size: % 8.4fMB    ", volumesize); 
    }
    else sprintf(boot_disp[boot_index++], "Size: % 8.4fkB    ", volumesize);
  }
  else{
    strcpy(boot_disp[boot_index++], "No FAT16/32 volume! ");
    warning_count += 1;
  }
  
  //Create directories for saving log files and boot info
  if(!SD.exists(boot_dir)){ //Don't run begin again if it has already been run - known bug in SD.h library: https://arduino.stackexchange.com/questions/3850/sd-begin-fails-second-time
    if(!SD.begin(chipSelect)){
      strcpy(boot_disp[boot_index++], "SD initialize failed");
      warning_count += 1;
    }
    if(!SD.exists(boot_dir)){ 
      if(!SD.mkdir(boot_dir)){
        strcpy(boot_disp[boot_index++], "SD.mkdir() failed!  ");
        warning_count += 1;
      }
    }
    if(!SD.exists(log_dir)){
      if(!SD.mkdir(log_dir)){
        strcpy(boot_disp[boot_index++], "SD.mkdir() failed!  ");
        warning_count += 1;
      }
    }
  }
  //Report number of warning encountered
  if(warning_count){
    sprintf(boot_disp[boot_index++], "% 2d Warnings on boot!", warning_count);
  }
  else{
    strcpy(boot_disp[boot_index++], "Success! 0 Warnings ");
  }
  
  //Save current boot log
  sprintf(current_file_name, "%02d%02d%02d%c%c.txt", year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count/26)+97, (log_file_count%26)+97); 
  boot_file_saved = saveToSD(boot_dir, current_file_name, (char *) boot_disp, boot_index, LCD_dim_x);
  if(!boot_file_saved){
    strcpy(boot_disp[boot_index++], "Boot save FAIL!     ");
  }
    
  if(Serial){
    for(int a = 0; a<boot_index; a++){
      for(int b=0; b<20; b++){
        Serial.print(boot_disp[a][b]);
      }
      Serial.println();
    }
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
  
    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
  }
  strcpy(boot_disp[boot_index++], "Use stick to scroll ");
  strcpy(boot_disp[boot_index++], "Press center to log ");
  LCD_window_lines[0] = boot_index;
  if(display_present){
    for(LCD_line_index = 0; LCD_line_index<boot_index-3; LCD_line_index++){
      for(int b=0; b<4; b++){
        lcd.setCursor(0,b);
        lcd.println(boot_disp[LCD_line_index+b]);        
      }
      delay(200);
    }
    lcd.setCursor(LCD_dim_x-2, 0);
    lcd.write(0);
  }
}

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

//Check coin cell voltage - since the ADC for the coin cell down is floating, we have to do a PULLDOWN to a known state first to see if a battery is connected before trying to measure the voltage
float checkVbat(){
  float coin_voltage = 0;
  pinMode(coin_analog_pin, INPUT_PULLDOWN);
  digitalWriteFast(coin_test_pin, HIGH); //Connect coin cell to ADC
  if(digitalRead(coin_analog_pin)){
    pinMode(coin_analog_pin, INPUT);
    coin_voltage = analogRead(coin_analog_pin);
    digitalWriteFast(coin_test_pin, LOW); //disconnect coin cell from ADC
    coin_voltage *= 3.3/analog_max;
  }
  return coin_voltage;
}

//Measure battery voltage, which is 2x ADC reading - due to the 1/2 voltage divider, ADC is in a known PULLDOWN state 
float checkVin(){
  float Vin_voltage = 0;
  pinMode(Vin_analog_pin, INPUT_PULLDOWN);
  digitalWriteFast(Vin_test_pin, HIGH); //Connect Vin cell to ADC
  if(digitalRead(Vin_analog_pin)){
    pinMode(Vin_analog_pin, INPUT);
    Vin_voltage = analogRead(Vin_analog_pin);
    digitalWriteFast(Vin_test_pin, LOW); //disconnect Vin cell from ADC
    Vin_voltage *= 6.6/analog_max;
  }
  return Vin_voltage;
}

//Save data to the SD card
boolean saveToSD(char (*dir), char (*file_name), char *data_array, int n_lines, int n_col){
  char file_path[100];
  char data_line[n_col+1];
  int i = -1;
  int j = -1;
  int dir_len;
  File f;

  

  //Concatenate directory and file name
  while(dir[++i]) file_path[i] = dir[i];
  file_path[i] = 0;
  if(SD.exists(file_path)){ //Make sure that the save directory exists before trying to save to it - without this check open() will lock without SD card  
    file_path[i] = '/';
    
    while(file_name[++j]) file_path[++i] = file_name[j];
    file_path[++i] = 0;
    
    //Open file
    f = SD.open(file_path, FILE_WRITE);
    if(f){
      for(int a=0; a<n_lines; a++){
        for(int b=0; b<n_col; b++){
          data_line[b] = *((data_array + a*n_col) + b);
        }
        data_line[n_col] = 0; //Force termination at end of string - prevent overlfow issues
        f.println(data_line);
      }
      f.close(); //File timestamp applied on close (save)
      return true;
    }
  }
  return false;
}
//Adjust LCD contrast
void setLCDcontrast(float contrast){
  //Force 0-1 range
  if(contrast > 1.0){
    contrast = 1.0;
  }
  else if(contrast < 0){
    contrast = 0;
  }
  int bit_contrast = round((1-contrast) * analog_max)>>2; //Values will automatically get mapped to max of 12-bit - https://www.pjrc.com/teensy/td_pulse.html; contrast is also inverted and maxes out at about 1/4 full range
  
  analogWrite(contrast_pin, bit_contrast);
}

//Adjust backlightintensity
void setLCDbacklight(float intensity){
  //Force 0-1 range
  if(intensity > 1.0){
    intensity = 1.0;
  }
  else if(intensity < 0){
    intensity = 0;
  }
  if(intensity > 0){
    int bit_intensity = round(intensity * analog_max);
    analogWrite(LED_PWM_pin, bit_intensity);
  }
  else{ //Fully shutdown 
    digitalWrite(LED_PWM_pin, LOW);
  }
}

//Fully shutdown display into lowest power state
void disableDisplay(){
  digitalWriteFast(LCD_toggle_pin, LOW); //Turn off LCD power
  digitalWriteFast(LED_PWM_pin, LOW); //Turn off backlight
  analogWrite(contrast_pin, 0); //Turn off LCD contrast
  DAC0_C0 = (unsigned char) ~DAC_C0_DACEN; //Disable DAC pin DAC0 to save power on hibernate - https://github.com/duff2013/Snooze/issues/12 - unsigned char to fix warning - https://www.avrfreaks.net/forum/warning-large-integer-implicitly-truncated-unsigned-type
}

//Check that LCD is connected and functioning correctly by moving cursor to all DDRAM addresses and confirming that busy flag returns that address
boolean monitorPresent(){
  uint8_t RAM_offset[] = {0, 0x40, 0x14, 0x54};
  uint8_t r; //Busy flag response from LCD
  uint8_t q; //Expected response from LCD
  //Check all DDRAM addresses and confrim checkBusy returns the same address
  for(int a=0; a<4; a++){
    for(int b=0; b<20; b++){
      lcd.setCursor(b,a);
      r = checkBusy(); 
      q = ((b+RAM_offset[a]) & 0xF0);
      if(r != q) return false;
    }
  }
  return true;
}

//Check if LCD is busy and return the DDRAM address
uint8_t checkBusy(){
  //Set DB pins as input
  for(int a=3; a>=0; a--) pinMode(LCD_pin[a+3], INPUT_PULLUP);
  delayMicroseconds(100); //must wait at least 80us before checking BF - https://www.newhavendisplay.com/app_notes/ST7066U.pdf
  
  //Send command to get busy flag
  digitalWriteFast(LCD_pin[0], LOW); //Send instruction
  digitalWriteFast(LCD_pin[1], HIGH); //Read
  digitalWriteFast(LCD_pin[2], HIGH); //Enable BF
  delayMicroseconds(100);
  
  //Get flag (DB7) and address counter (DB4-DB6)
  uint8_t response = 0;
  for(int a=3; a>=0; a--){
    if(digitalRead(LCD_pin[a+3])) bitSet(response, a+4); //shift 4 MSB bits to MSB position
  }
  digitalWriteFast(LCD_pin[2], LOW); //Reset BF
  delayMicroseconds(2);
  digitalWriteFast(LCD_pin[2], HIGH); //Enable BF
  delayMicroseconds(100);
  digitalWriteFast(LCD_pin[2], LOW); //Reset BF
  for(int a=3; a>=0; a--) pinMode(LCD_pin[a+3], OUTPUT);
  return response;
}

void wakeUSB(){
    elapsedMillis time = 0;
    while (!Serial && time < 1000) {
        Serial.write(0x00);// print out a bunch of NULLS to serial monitor
        digitalWriteFast(LED_BUILTIN, HIGH);
        delay(30);
        digitalWriteFast(LED_BUILTIN, LOW);
        delay(30);
    }
    // normal delay for Arduino Serial Monitor
    delay(200);
}
