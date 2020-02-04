#include "Adafruit_BME280.h" //Temp, pressure, humidity sensor
#include "Adafruit_Sensor.h" //Universal sensor API
#include "Adafruit_TCS34725.h" //Color Sensor
#include "Adafruit_TSL2591.h" //Light Sensor
#include "LCD.h" //Display
#include <Wire.h> //I2C
#include <TimeLib.h> //Set RTC time and get time strings
#include <Snooze.h> //Put Teensy into low power state between log points
#include <SD.h> //Store data onto SD card

//Setup default parameters
int n_logs_per_file = 10000; //The number of logs to save to a file before creating a new low file
int log_interval = 10; //Number of interval units between logs
char log_interval_unit[] = "seconds"; //Valid interval units - "seconds", "minutes", "hours", "days", "months", "years"
const char boot_dir[] = "boot_log"; //Directory to save boot log files into - max length 8 char
const char log_dir[] = "data_log"; //Directory to save data log files into - max length 8 char
boolean measure_temp = true;
boolean measure_humidity = true;
boolean measure_pressure = true;
boolean measure_lux = true;
boolean measure_color = true;
boolean measure_battery = true;

boolean backlight_on_standby = false; //Turn on backlight when not logging
boolean backlight_on_log = false; //Turn on backlight while logging
boolean display_on_standby = true; //Turn on display when not logging 
boolean display_on_log = false; //Turn on display while logging

//Setup LCD pin numbers and initial parameters
int DB_pin_array[] = {32, 31, 8, 6, 5, 4, 3, 1}; //List of DB0-DB7 pins to send data to LCD - 4-pin is not fully supported
boolean four_pin_mode = false; //Whether to run the display in 4-pin mode to free up pins - may not be stable
int RS_pin = 30;
int RW_pin = 34;
int E_pin = 35;
int LCD_toggle_pin = 24; //Set to high to power on LCD
int LED_PWM_pin = 29; //Drive LED backlight intensity
int contrast_pin = A21; //DAC pin for addjusting diplay contrast
int n_DB_pin = sizeof(DB_pin_array)/sizeof(DB_pin_array[0]);
int analog_resolution = 16; //Number of bits in PWM and DAC analog  - PWM cap is 16, DAC cap is 12 - auto capped in code - https://www.pjrc.com/teensy/td_pulse.html
int analog_max = (1<<analog_resolution)-1; //Highest analog value
int analog_freq = 24000000/(1<<analog_resolution); //Calculate freq based on fastest for minimum clock speed - 24 MHz
float default_backlight = 1.0; //Set default backlight intensity to full brightness (range is 0-1)
float default_contrast = 0.5; //Set default LCD contrast to half range (range is 0-1)
                        
//Setup sensor pin numbers
int temp_power_pin = 20; //Set Vcc pins
int color_power_pin = 23;
int light_power_pin = 17;
int color_interrupt_pin = 16; //Set interrupt pins
int light_interrupt_pin = 33;
int I2C_pullup_pin = 12; //Pin providing I2C pullup voltage
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Setup battery test pin numbers
int coin_test_pin = 36;
int coin_analog_pin = A1;
int Vin_test_pin = 39;
int Vin_analog_pin = A0; 

//Setup SD card
const int chipSelect = BUILTIN_SDCARD;
Sd2Card card;
SdVolume volume;
SdFile root;

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

//File status
boolean boot_log_saved = false; //Whether the boot log has been saved to the SD card
int log_index = 0; //Index of log array
const char log_header[] = "Date,Time,Temperature (°C),Pressure (hPa),Humidity (%),Light Intensity (lux),Red (µW/cm^2),Green (µW/cm^2),Blue (µW/cm^2),Clear (µW/cm^2),Vin (V),Vbat (V),Comment,";
const int boot_dim1 = 20; //Number of rows (total lines)
const int boot_dim2 = 20; //Number of columns (characters per line)
char boot_array[boot_dim1][boot_dim2]; //Array for boot display
int boot_index = 0; //Index of boot message
int warning_count = 0; //Number of warnings encountered during boot 
int time_index = 0; //Index of boot_array where time stamp is stored
const int log_dim1 = 1000; //Number of rows (total lines)
const int log_dim2 = 100; //Number of columns (characters per line)
volatile char internal_log_backup[1000][100]; //Array for storing data logs while SD card is not available
char current_file_name[13]; //Storing the current log file name - max length for FAT32 is 8.3 - 13 characters total including null at end
int log_file_count = 0; //Counter for tracking number of log files in ascii (track files from aa to zz)

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_60X); //Create instance of color sensor initialize with peak sensitivity
Adafruit_TSL2591 light_sensor = Adafruit_TSL2591(2591); //Create instance of light sensor - number is sensor ID
LCD lcd(DB_pin_array, n_DB_pin, RS_pin, RW_pin, E_pin, LCD_toggle_pin, LED_PWM_pin, contrast_pin, analog_resolution); //Create instance of LCD display

//------------------------------------------------------------------------------
// call back for file timestamps
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
  strcpy(boot_array[boot_index++], "Boot status:        ");

  //Turn off Teensy LED
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWriteFast(LED_BUILTIN, LOW);
  
  //Power on the sensors
  pinMode(temp_power_pin, OUTPUT);
  pinMode(color_power_pin, OUTPUT);
  pinMode(light_power_pin, OUTPUT);
  pinMode(I2C_pullup_pin, OUTPUT);
  digitalWriteFast(temp_power_pin, HIGH);
  digitalWriteFast(color_power_pin, HIGH);
  digitalWriteFast(light_power_pin, HIGH);
  digitalWriteFast(I2C_pullup_pin, HIGH);

  //Set battery test pins
  pinMode(coin_test_pin, OUTPUT);
  pinMode(Vin_test_pin, OUTPUT);
  digitalWriteFast(coin_test_pin, LOW); //Disconnect coin cell from ADC
  digitalWriteFast(Vin_test_pin, LOW); //Disconnect battery from ADC
  pinMode(coin_analog_pin, INPUT); //Set coin cell ADC to floating high impedance
  pinMode(Vin_analog_pin, INPUT); //Set battery ADC to floating high impedance
 
  //Setup USB serial communication
  Serial.begin(9600); //Baud rate is ignored and negotiated with computer for max speed
  wakeUSB();
  
  //Setup I2C communication to highest speed chips sill support - sensor libraries will initialize I2C communications
  Wire.setClock(400000); 
  Wire1.setClock(400000);

  //Start LCD display
  analogWriteResolution(analog_resolution); //Set DAC and PWM resolution - NOTE: Do not change once set!
  analogReadResolution(analog_resolution); //Set ADC resolution - NOTE: Do not change once set!
  analogWriteFrequency(LED_PWM_pin, analog_freq); //Set LED PWM freq - other pins on same timer will also change - https://www.pjrc.com/teensy/td_pulse.html
  if(four_pin_mode){
    for(int a=0; a<n_DB_pin; a++){
      if(a<4) pinMode(DB_pin_array[a], INPUT); //Temporarily set the LSB pins to high impedance so the display can boot into 4-bit mode
      else DB_pin_array[a-4] = DB_pin_array[a]; //Shift the MSB pins to the LSB slots
    }
    lcd = LCD(DB_pin_array, 4, RS_pin, RW_pin, E_pin, LCD_toggle_pin, LED_PWM_pin, contrast_pin, analog_resolution); //Overwrite the LCD instance
  }
  lcd.setLCDbacklight(default_backlight); //Turn on LED backlight to default intensity
  lcd.setLCDcontrast(default_contrast); //Initialize screen contrast to default value
  if(lcd.initializeLCD()){ //Start LCD display and confirm if present
    strcpy(boot_array[boot_index++], "Display initialized ");
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
    strcpy(boot_array[boot_index++], "Display not found!  ");
    warning_count += 1;
    display_present = false;
    display_on = false;
    backlight_on = false;
  }

  //Synchronize Teensy clock to computer clock
  setSyncProvider(getTeensy3Time); //Sync to computer clock - do not use () in function as argument
  if (timeStatus()!= timeSet) { //If sync failed
    strcpy(boot_array[boot_index++], "RTC time sync fail! ");
    warning_count += 1;
  } else { // if Sync successful
    strcpy(boot_array[boot_index++], "RTC sync successful ");
  }
  unix_t = now();
  sprintf(boot_array[boot_index++], "%4d/%02d/%02d %02d:%02d:%02d ", year(unix_t), month(unix_t), day(unix_t), hour(unix_t), minute(unix_t), second(unix_t)); //Write current time to boot screen

  //Initialize sensors
  if(temp_sensor.begin(0x77, temp_port)){
    strcpy(boot_array[boot_index++], "Temp sensor....OK   ");
    temp_present = true;
    temp_on = true;
  }
  else{
    strcpy(boot_array[boot_index++], "Temp sensor....FAIL!");
    warning_count += 1;
    digitalWriteFast(temp_power_pin, LOW); //Cut power to sensor
    temp_present = false;
    temp_on = false;
  }
  if(color_sensor.begin(0x29, color_port)){
    strcpy(boot_array[boot_index++], "Color sensor...OK  ");
    color_present = true;
    color_on = true;
  }
  else{
    strcpy(boot_array[boot_index++], "Color sensor...FAIL!");
    warning_count += 1;
    digitalWriteFast(color_power_pin, LOW); //Cut power to sensor
    color_present = false;
    color_on = false;
  }
  if(light_sensor.begin(light_port)){
    strcpy(boot_array[boot_index++], "Light sensor...OK   ");
    light_present = true;
    light_on = true;    
  }
  else{
    strcpy(boot_array[boot_index++], "Light sensor...FAIL!");
    warning_count += 1;
    digitalWriteFast(light_power_pin, LOW); //Cut power to sensor
    light_present = false;
    light_on = false;  
  }
  digitalWriteFast(I2C_pullup_pin, LOW); //Power down I2C pullup as sensor communication is complete
  
  //Test batteries
  float volt = checkVbat();
  sprintf(boot_array[boot_index++], "VBat: %4.2fV         ", volt);
  if(volt > 0){
    coin_present = true;
    if(volt < 2.8){
      strcpy(boot_array[boot_index++], "Vbat low voltage!   ");
      warning_count += 1;
    }
  }

  volt = checkVin();
  sprintf(boot_array[boot_index++], "Vin:  %4.2fV         ", volt);
  if(volt < 3.6){
    strcpy(boot_array[boot_index++], "Vin low voltage!    ");
    warning_count += 1;
  }
  
  // set date time callback function for applying RTC synced time stamps to SD card file time stamps
  SdFile::dateTimeCallback(dateTime);

  //Test SD card
  if (card.init(SPI_HALF_SPEED, chipSelect)) { //Check if SD card is present
    strcpy(boot_array[boot_index++], "SD card found       ");
  }
  else{
    strcpy(boot_array[boot_index++], "SD card not found!  ");
    warning_count += 1;
  }
  if (volume.init(card)) { //Check that there is a FAT32 partition
    int partition = volume.fatType();
    float volumesize;
    sprintf(boot_array[boot_index++], "FAT%2d volume found  ", partition);
    volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
    volumesize *= volume.clusterCount();       // we'll have a lot of clusters
    volumesize /= 2;
    if(volumesize >= 1e3){ //If volume is more than 1MB in size - report size as MB
      volumesize /= 1000;
      if(volumesize >= 1e3){ //If volume is more than 1GB in size - report size as GB
        volumesize /= 1000;
        if(volumesize >= 1e3){ //If volume is more than 1TB in size - report size as TB
          volumesize /= 1000;
          sprintf(boot_array[boot_index++], "Size: % 8.4fTB    ", volumesize); 
        }
        else{
          sprintf(boot_array[boot_index++], "Size: % 8.4fGB    ", volumesize); 
        }
      }
      else{
        sprintf(boot_array[boot_index++], "Size: % 8.4fMB    ", volumesize); 
      }
    }
    else{
      sprintf(boot_array[boot_index++], "Size: % 8.4fkB    ", volumesize);
    }
  }
  else{
    strcpy(boot_array[boot_index++], "No FAT16/32 volume! ");
    warning_count += 1;
  }
  
  //Create directories for saving log files and boot info
  if(!SD.begin(chipSelect)){
    strcpy(boot_array[boot_index++], "SD initialize failed");
    warning_count += 1;
  }
  if(!SD.exists(boot_dir)){ 
    if(!SD.mkdir(boot_dir)){
      strcpy(boot_array[boot_index++], "SD.mkdir() failed!  ");
      warning_count += 1;
    }
  }
  if(!SD.exists(log_dir)){
    if(!SD.mkdir(log_dir)){
      strcpy(boot_array[boot_index++], "SD.mkdir() failed!  ");
      warning_count += 1;
    }
  }

  //Save current boot log
  sprintf(current_file_name, "%02d%02d%02d%c%c.txt", year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count%26)+97, (log_file_count/26)+97); 
  if(log_file_count++ >= 26*26) log_file_count = 0;
  Serial.println(current_file_name);
  if(!saveToSD(boot_dir, current_file_name, (char *) boot_array, boot_index, boot_dim2)){
    strcpy(boot_array[boot_index++], "Boot save FAIL!     ");
    warning_count += 1; 
  }
    
  if(Serial){
    for(int a = 0; a<boot_index; a++){
      for(int b=0; b<20; b++){
        Serial.print(boot_array[a][b]);
      }
      Serial.println();
    }
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
  
    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
  }
  if(display_present){
    for(int a = 0; a<boot_index-3; a++){
      lcd.displayCharArray(boot_array, a, a+1, a+2, a+3);
      delay(200);
    }
  }
}

void loop() {
  delay(100);
  digitalWriteFast(LED_BUILTIN, HIGH);
  delayMicroseconds(100);
  digitalWriteFast(LED_BUILTIN, LOW);
  delayMicroseconds(100);
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
  char file_path[500];
  char data_line[n_col];
  int i = -1;
  int j = -1;
  int dir_len;
  File f;

  //Concatenate directory and file name
  while(dir[++i]) file_path[i] = dir[i];
  file_path[i] = '/';
  while(file_name[++j]) file_path[++i] = file_name[j];
  file_path[++i] = 0;
  Serial.println(file_path);
  Serial.println(file_name);
  //Open file
  f = SD.open(file_path, FILE_WRITE);
  if(f){
    for(int a=0; a<n_lines; a++){
      for(int b=0; b<n_col; b++){
        data_line[b] = *((data_array + a*n_col) + b);
      }
      f.println(data_line);
      Serial.print("DATA: ");
      Serial.println(data_line);
    }
    f.close();
    return true;
  }
  return false;
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
