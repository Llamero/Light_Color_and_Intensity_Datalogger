#include "Adafruit_BME280.h" //Temp, pressure, humidity sensor
#include "Adafruit_Sensor.h" //Universal sensor API
#include "Adafruit_TCS34725.h" //Color Sensor
#include "Adafruit_TSL2591.h" //Light Sensor
#include "LiquidCrystalFast.h" //Display
#include <Wire.h> //I2C
#include <TimeLib.h> //Set RTC time and get time strings
#include <Snooze.h> //Put Teensy into low power state between log points
#include <SD.h> //Store data onto SD card

//Define software restart
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

//Setup default parameters
uint8_t log_interval_array[] = {0, 0, 2}; //Number of hours, minutes, and seconds between log intervals
const char boot_dir[] = "boot_log"; //Directory to save boot log files into - max length 8 char
const char log_dir[] = "data_log"; //Directory to save data log files into - max length 8 char
boolean measure_temp = true;
boolean measure_humidity = true;
boolean measure_pressure = true;
boolean measure_lux = true;
boolean measure_color = true;
boolean measure_battery = true;

boolean disable_display_on_log = true; //Completely power down display during logging
boolean save_on_log_interval = false; //Save to SD every log interval - this is less battery efficient during saves than saving in 512 byte blocks, but will ensure no loss of data 
const float default_backlight = 0; //Set default backlight intensity to full brightness (range is 0-1)
const float default_contrast = 0.7; //Set default LCD contrast to half range (range is 0-1)

//Allow Teensy to re-run setup when settings are changed - Call "CPU_RESTART"
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// Load drivers
SnoozeDigital digital;
SnoozeTimer timer;
SnoozeBlock hibernate_config(digital, timer);
time_t unix_t = 0; //Track current device time
uint16_t unix_ms = 0; //Current time in ms
time_t next_log_time = 0; //next log time in unix time
time_t time_remaining = 0; //Time remaing to next log interval
time_t log_interval = (60*60*log_interval_array[0])+(60*log_interval_array[1])+log_interval_array[2];
const uint16_t log_offset = 1000; //Time in ms it takes to log a data point - also determines smallest log interval - can't be >5535 ms.
boolean log_next_wake = false; //Whether to log on the next timer wake event

//Setup LCD pin numbers and initial parameters
const uint8_t LCD_pin[] = {30, 32, 31, 5, 3, 1, 0}; //Only 4-pin supported in LiquidCrystalFast
              // LCD pins: RS  RW  EN  D4 D5 D6 D7
LiquidCrystalFast lcd(LCD_pin[0], LCD_pin[1], LCD_pin[2], LCD_pin[3], LCD_pin[4], LCD_pin[5], LCD_pin[6]);
const uint8_t LCD_toggle_pin = 29; //Set to high to power on LCD
const uint8_t LED_PWM_pin = 10; //Drive LED backlight intensity
const uint8_t contrast_pin = A21; //DAC pin for addjusting diplay contrast
const uint8_t analog_resolution = 16; //Number of bits in PWM and DAC analog  - PWM cap is 16, DAC cap is 12 - auto capped in code - https://www.pjrc.com/teensy/td_pulse.html
const uint16_t analog_max = (1<<analog_resolution)-1; //Highest analog value
const uint32_t analog_freq = 24000000/(1<<analog_resolution); //Calculate freq based on fastest for minimum clock speed - 24 MHz

//Define LCD custom characters - from https://www.instructables.com/id/Custom-Large-Font-For-16x2-LCDs/
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


const uint8_t  gBigFontShapeTable[]  = {
//* LT[8] =
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
//* UB[8] =
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
//* RT[8] =
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
//* LL[8] =
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111,
//* LB[8] =
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
//* LR[8] =
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100,
//* UMB[8] =
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
//* LMB[8] =
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111


};


//************************************************************************
//* 6 numbers for each character
//* 9 means BLANK
const uint8_t gBigFontAsciiTable[]  = {

  32, 32, 32, 32, 32, 32,   //  0x20  space
  32, 0,  32, 32, 4,  32,   //  0x21  !
  32, 32, 32, 32, 32, 32,   //  0x22
  32, 32, 32, 32, 32, 32,   //  0x23
  32, 32, 32, 32, 32, 32,   //  0x24
  32, 32, 32, 32, 32, 32,   //  0x25
  32, 32, 32, 32, 32, 32,   //  0x26
  32, 32, 32, 32, 32, 32,   //  0x27
  32, 32, 32, 32, 32, 32,   //  0x28
  32, 32, 32, 32, 32, 32,   //  0x29
  32, 32, 32, 32, 32, 32,   //  0x2A
  32, 32, 32, 32, 32, 32,   //  0x2B
  32, 32, 32, 32, 32, 32,   //  0x2C
  32, 32, 32, 32, 32, 32,   //  0x2D
  32, 32, 32, 32, 4,  32,   //  0x2E  . (period)
  32, 32, 32, 32, 32, 32,   //  0x2F


  0,  1,  2,  3,  4,  5,    //  0x30  0
  1,  2,  32, 32, 5,  32,   //  0x31  1
  6,  6,  2,  3,  7,  7,    //  0x32  2
  6,  6,  2,  7,  7,  5,    //  0x33  3
  3,  4,  2,  32, 32, 5,    //  0x34  4
  255,6,  6,  7,  7,  5,    //  0x35  5
//  0,  6,  6,  7,  7,  5,    //  0x35  5
  0,  6,  6,  3,  7,  5,    //  0x36  6
  1,  1,  2,  32, 0,  32,   //  0x37  7
  0,  6,  2,  3,  7,  5,    //  0x38  8
  0,  6,  2,  32, 32, 5,    //  0x39  9
  32, 32, 32, 32, 32, 32,   //  0x3A
  32, 32, 32, 32, 32, 32,   //  0x3B
  32, 32, 32, 32, 32, 32,   //  0x3C
  32, 32, 32, 32, 32, 32,   //  0x3D
  32, 32, 32, 32, 32, 32,   //  0x3E
  1,  6,  2,  254,7,  32,   //  0x3F  ?

  32, 32, 32, 32, 32, 32,   //  0x40  @
  0,  6,  2,  255,254,255,  //  0x41  A
  255,6,  5,  255,7,  2,    //  0x42  B
  0,  1,  1,  3,  4,  4,    //  0x43  C
  255,1,  2,  255,4,  5,    //  0x44  D
  255,6,  6,  255,7,  7,    //  0x45  E
  255,6,  6,  255,32, 32,   //  0x46  F

  0,  1,  1,  3,  4,  2,    //  0x47  G
  255,4,  255,255,254,255,  //  0x48  H
  1,  255,1,  4,  255,4,    //  0x49  I
  32, 32, 255,4,  4,  5,    //  0x4A  J
  255,4,  5,  255,254,2,    //  0x4B  K
  255,32, 32, 255,4,  4,    //  0x4C  L
  32, 32, 32, 32, 32, 32,   //  0x4D  M place holder
  32, 32, 32, 32, 32, 32,   //  0x4E  N place holder
  0,  1,  2,  3,  4,  5,    //  0x4F  O (same as zero)

  0,  6,  2,  3,  32, 32,   //  0x50  P
  32, 32, 32, 32, 32, 32,   //  0x51  Q
  0,  6,  5,  3,  32, 2,    //  0x52  R
  0,  6,  6,  7,  7,  5,    //  0x35  S (same as 5)
  1,  2,  1,  32, 5,  32,   //  0x54  T
  2,  32, 2,  3,  4,  5,    //  0x55  U
  32, 32, 32, 32, 32, 32,   //  0x56  V place holder
  32, 32, 32, 32, 32, 32,   //  0x57  W place holder
  3,  4,  5,  0,  32, 2,    //  0x58  X
  3,  4,  5,  32, 5,  32,   //  0x59  Y
  1,  6,  5,  0,  7,  4,    //  0x5A  Z
  0

};

//* we have a seperate table for the wide routines
const uint8_t gBigFontAsciiTableWide[]  = {
//* this table is 10 bytes, 2 rows of 5
//  ---top------------|
  0,  1,  3,  1,  2,  3,  32, 32, 32, 5,    //  0x4D  M   5-wide
  0,  3,  32, 2,  32, 3,  32, 2,  5,  32,   //  0x4E  N   4-wide
  0,  1,  2,  32, 32, 3,  4,  3,  4,  32,   //  0x51  Q   4-wide
  3,  32, 32, 5,  32, 32, 3,  5,  32, 32,   //  0x56  V   4-wide
  0,  32, 32, 32, 2,  3,  4,  0,  4,  5,    //  0x57  W   5-wide
  0
};


//Setup sensor pin numbers
const uint8_t temp_power_pin = 20; //Set Vcc pins
const uint8_t color_power_pin = 23;
const uint8_t light_power_pin = 17;
const uint8_t color_interrupt_pin = 16; //Set interrupt pins
const uint8_t light_interrupt_pin = 21;
const uint8_t I2C_pullup_pin = 8; //Pin providing I2C pullup voltage
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Sensor settings:
const float color_gain_value[] = {1, 4, 16, 60};
const uint8_t color_gain_command[] = {0x00, 0x01, 0x02, 0x03}; 
const uint8_t color_max_index = sizeof(color_gain_value)/sizeof(color_gain_value[0])-1;
uint8_t color_gain_index = color_max_index; //Start at maximum sensitivity
const uint16_t color_integration_value[] = {24, 101, 154, 700};
const uint8_t color_integration_command[] = {0xF6, 0xD5, 0xC0, 0x00};
const uint16_t color_integration_max_count[] = {10240, 43008, 65535, 65535}; //Max ADC count for given integration time 
uint8_t color_integration_index = color_max_index; //Start at maximum sensitivity
const uint8_t color_over_shift = 1; //Over-saturated cutoff - number of shifts to max value (= divide by 2 per shift)
const uint8_t color_under_shift = color_over_shift + 3; //Exposure steps in 2^2 so min is 2 + 1 = >>3 
uint16_t color_over_exposed = color_integration_max_count[color_integration_index] >> color_over_shift;
uint16_t color_under_exposed = color_integration_max_count[color_integration_index] >> color_under_shift;

const float vis_gain_value[] = {1, 24.5, 400, 9200};
const float IR_gain_value[] = {1, 24.5, 400, 9900};
const uint8_t light_gain_command[] = {0x00, 0x10, 0x20, 0x30}; 
const uint8_t light_max_index = sizeof(vis_gain_value)/sizeof(vis_gain_value[0])-1;
uint8_t light_gain_index = light_max_index; //Start at maximum sensitivity
const uint16_t light_integration_value[] = {100, 200, 400, 600};
const uint8_t light_integration_command[] = {0x00, 0x01, 0x03, 0x05};
const uint16_t light_integration_max_count[] = {36863, 65535, 65535, 65535}; //Max ADC count for given integration time 
uint8_t light_integration_index = light_max_index; //Start at maximum sensitivity
const uint8_t light_over_shift = 1; //Over-saturated cutoff - number of shifts to max value (= divide by 2 per shift)
const uint8_t light_under_shift = light_over_shift + 6; //Gain steps in 2^5 so min is 5 + 1 = >>6 
uint16_t light_over_exposed = light_integration_max_count[light_integration_index] >> light_over_shift;
uint16_t light_under_exposed = light_integration_max_count[light_integration_index] >> light_under_shift;

//Setup joystick pins
const uint8_t joystick_pins[] = {9, 6, 2, 7, 4}; //Joystick pins - up, right, down, left, push 
                              //u,  r,  d, l, center 
const uint8_t debounce = 50; //Time in ms to debounce button press

//Setup battery test pin numbers
const uint8_t coin_analog_pin = A0;
const uint8_t Vin_test_pin = 11;
const uint8_t Vin_analog_pin = A1; 

//Setup SD card
const int chipSelect = BUILTIN_SDCARD;
Sd2Card card;
SdVolume volume;
SdFile root;
File f;
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
uint8_t wakeup_source; //Source of wake from hibernate

//File status
const uint8_t boot_dim_y = 20; //Number of rows (total lines)
const uint8_t LCD_dim_x = 21; //Number of columns (characters per line) - add one character for null character to delineate strings
char boot_disp[boot_dim_y][LCD_dim_x]; //Array for boot display
const uint8_t log_disp_dim_y = 24; //Number of rows (total lines)
char log_disp[log_disp_dim_y][LCD_dim_x] = {"Log Status:         ","---------RTC--------","Date:               ","Time:               ","-----AIR SENSOR-----","Temp:             \xDF""C","Pres(hPa):          ","Humidity(%):        ","----LIGHT SENSOR----","Gain:               ","Integration:      ms","Vis ADC:            ","IR ADC:             ","----COLOR SENSOR----","Gain:               ","Integration:      ms","Red ADC:            ","Green ADC:          ","Blue ADC:           ","Clear ADC:          ","-------BATTERY------","Vin:               V","Vbat:              V","Comment:            "};
const uint8_t settings_dim_y = 17; //Number of rows (total lines)
char settings_disp[settings_dim_y][LCD_dim_x] = {"Settings:           ","-------SENSORS------","Temperature:        ","Humidity:           ","Pressure:           ","Light:              ","Color:              ","Battery:            ","----LOG INTERVAL----", "(hh:mm:ss):         ","----LCD SETTINGS----","Contrast:           ","Backlight:          ","Disable on log:     ","----RTC SETTINGS----","Date:               ","Time:               "};
const uint8_t n_windows = 3;
char *LCD_windows[] = {(char *) boot_disp, (char *) log_disp, (char *) settings_disp}; //Array of windows available for LCD
uint8_t LCD_window_index = 0; //Current LCD window being displayed
uint8_t LCD_line_index = 0; //Current index of top line being displayed
uint8_t LCD_window_lines[] = {boot_dim_y, log_disp_dim_y, settings_dim_y}; //Array of total number of lines for each window
const char log_header[] = "Date,Time,Temperature(°C),Pressure(hPa),Humidity(%),Light_Full_Gain,Light_IR_Gain,Light_Integration(ms),Light_Full_ADC,Light_IR_ADC,RGB_Gain,RGB_Integration(ms),Red_ADC,Green_ADC,Blue_ADC,Clear_ADC,Vin(V),Vbat(V),Comment,";
const uint8_t n_log_columns = 13; //Number of items to log per round
uint8_t boot_index = 0; //Index of boot message
int warning_count = 0; //Number of warnings encountered during boot 
const uint32_t log_internal_buffer_size = 200000;
volatile char log_internal_buffer[log_internal_buffer_size]; //Array for storing data logs while SD card is not available  - must be 2^16 as a ciruclar buffer is used to ensure efficient SD storage (512 byte blocks) - use uint16_t rollover to implement circular buffer
uint32_t log_start_index = 0; //Index where previous write to SD finished - allows writing to SD in blocks of 512 bytes
uint32_t log_end_index = log_start_index; //Index of internal log string - use uint16_t rollover to implement circular buffer
const char *log_reset_pointer = log_internal_buffer + (log_internal_buffer_size - 512); //The address in the buffer where the buffer needs to be reset to avoid overflow
char current_boot_file_path[25]; //Storing the current log file path - max length for FAT32 is 8.3 - 13 characters total including null at end
char current_log_file_path[25]; //Storing the current log file name - max length for FAT32 is 8.3 - 13 characters total including null at end
uint16_t log_line_count = 0; //Number of rows in current log file
uint16_t log_file_count = 0; //Counter for tracking number of log files in ascii (track files from aa to zz) - sets file suffix to next availalble - AA->AB->AC, etc.
const uint16_t max_log_file = 26*26; //Largest number of log files available on the same date
boolean log_active = false; //Whether the device is actively logging or in standby state

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(color_integration_command[color_integration_index], color_gain_command[color_gain_index]); //Create instance of color sensor initialize with peak sensitivity
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


float contrast = 0;
boolean state = false;
uint32_t log_display_counter = 0;

void setup() {
  initializeDevice();
}

void loop() {
  uint32_t index = 0;
  log_display_counter++;
//  lcd.setCursor(0,0);
//  lcd.print(next_log_time);
//  lcd.setCursor(0,2);
//  lcd.print(RTCms());
//  lcd.setCursor(0,1);
//  lcd.print(unix_t);
//  //If an initial timer time has been set, increment timer time
  if(!wakeup_source){
    pinMode(joystick_pins[4], INPUT_PULLUP);
    if(!digitalRead(joystick_pins[4])) wakeup_source = joystick_pins[4]; //Check for stop press before sleeping
    else wakeup_source = Snooze.hibernate(hibernate_config);
  }
  RTCms();
  if(wakeup_source <= 33) delay(debounce);
  wakeupEvent(wakeup_source);
//  if(log_active){
//    if(!display_on) disableDisplay(false);
//    lcd.setCursor(0,0);
//    lcd.print(log_display_counter);
//  }  
}

void wakeupEvent(uint8_t src){
  //joystick_pins[] = {9, 11, 2, 7, 10}; //Joystick pins - up, right, down, left, push 
  if(src > 33){ //If > 33 then trigger was a non-digital event such as RTC timer   
    wakeup_source = 0; //Clear wake-up source
    if(log_next_wake){
      if(log_active){ 
         logEvent();    
      }
      next_log_time += log_interval; //Increment log time to next time point - if point was missed, go to next point
    }
    setHibernateTimer(); //Reset wakeup timer to next interval
    return;
  }
  
  pinMode(src, INPUT_PULLUP); //Set pin to state where it can be queried
  if(display_present){ //Lateral joystick inputs are only valid if there is a display
    if(src == joystick_pins[0] || src == joystick_pins[2]){
      scrollWindow(src);
    }
    else if(src == joystick_pins[1] || src == joystick_pins[3]){
      cycleWindow(src);
    }
    wakeup_source = 0; //Clear wake-up source
  }
  if(src == joystick_pins[4]){
    centerPress(src);
    wakeup_source = 0;
  }
  //If wake event was button press, wait for button release
  if(src <= 33){
    while(!digitalRead(src));
    delay(debounce);
  }
}

void centerPress(uint8_t src){
  //If center-press is held for more than 5 senconds, toggle log
  char lcd_timer = '5';
  char line1[] = "START";
  char line2[] = "LOG";
  uint8_t index = 0;
  uint8_t col = 0;
  if(log_active) strcpy(line1, "STOP ");
  if(disable_display_on_log && log_active) disableDisplay(false);
  BigNumber_SendCustomChars(); //Export double line char table - https://www.instructables.com/id/Custom-Large-Font-For-16x2-LCDs/
  timer.setTimer(1000);

  pinMode(src, INPUT_PULLUP);//Set pin so that it can be checked if it is still pressed
  if(SD.exists(boot_dir)){
    while(lcd_timer > '0' && !digitalRead(src)){
      lcd.clear();
      index = 0;
      col = 0;  
      while(line1[index]) col += DrawBigChar(col, 0, line1[index++]);
      index = 0;
      col = 0;
      while(line2[index]) col += DrawBigChar(col, 2, line2[index++]);
      DrawBigChar(15, 2, lcd_timer);
      wakeup_source = Snooze.hibernate(hibernate_config);
      if(wakeup_source == src) break;
      else if(wakeup_source < 34) wakeup_source = Snooze.hibernate(hibernate_config);
      else lcd_timer--;
      pinMode(src, INPUT_PULLUP); //Set pin so that it can be checked if it is still pressed
    }
    
    if(lcd_timer == '0'){   
      toggleLog();
    }
    else{
      if(disable_display_on_log && log_active) disableDisplay(true);
      if(!log_active) scrollWindow(src); 
    }
  
    //Restore arrow chars for menu scrolling
    lcd.createChar(0, up_arrow); //Create arrow characters
    lcd.createChar(1, down_arrow);
  }
  else noSdHold();
}

//Increment timer and sync to RTC;
void setHibernateTimer(){
  uint32_t read1, read2, secs, us;
  uint16_t ms_remaining = 0;
  RTCms(); //Get current time
  
  if(next_log_time > unix_t){ //If time is still remaining
    if(time_remaining >= 64){
      timer.setTimer(60000);
      log_next_wake = false;
    }
    else{
      RTCms(); //Get RTCms first as it will also automatically update unix_t if a rollover happened
      ms_remaining = ((next_log_time - unix_t) * 1000) - unix_ms; //calculate ms remaining to next log time
      timer.setTimer(ms_remaining);
      log_next_wake = true;
    }
  }
  else{ //If no time is left - sleep a short interval and log
    timer.setTimer(10);
    log_next_wake = true;
  }
}

//Handles starting and stopping logs
void toggleLog(){
  log_active = !log_active;
  if(!display_present){ //If there is not a display - show one LED flash for log start and two LED flashes for log stop
    pinMode(LED_BUILTIN, OUTPUT); 
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWriteFast(LED_BUILTIN, LOW);
    if(!log_active){
      delay(1000); 
      digitalWriteFast(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWriteFast(LED_BUILTIN, LOW);
    }
  } 
  if(log_active){
    if(disable_display_on_log) disableDisplay(true);
    initializeLog();
  }
  else{
    log_start_index = saveToSD(current_log_file_path, (char *) log_internal_buffer, log_start_index, log_end_index, true); //Save remainder of current file
    RTCms();
    for(log_file_count = 0; log_file_count < max_log_file-1; log_file_count++){ //Look for next available file name
      sprintf(current_log_file_path, "%s/%02d%02d%02d%c%c.csv", log_dir, year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count/26)+97, (log_file_count%26)+97);     
      if(!SD.exists(current_log_file_path)) break;
    }
    scrollWindow(joystick_pins[4]); //Restore display
  }
}

void logEvent(){
  uint16_t RTC_ms, r, g, b, c, full, IR, sensor_delay;
  uint32_t lum, sensor_finished;
  float temp, pres, hum, Vbat, Vin;
  char response = ' ';

  log_line_count++; //Increment log counter
  if(!log_line_count){ //If log line counter has rolled over - save rest of current buffer and then start new log file
    log_start_index = saveToSD(current_log_file_path, (char *) log_internal_buffer, log_start_index, log_end_index, true); //Save remainder of current file
    RTCms();
    for(log_file_count = 0; log_file_count < max_log_file-1; log_file_count++){ //Look for next available file name
      sprintf(current_log_file_path, "%s/%02d%02d%02d%c%c.csv", log_dir, year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count/26)+97, (log_file_count%26)+97);     
      if(!SD.exists(current_log_file_path)) break;
    }
    initializeLog();  
  }
  else{
    //Start up sensor readings
    digitalWriteFast(I2C_pullup_pin, HIGH); //Pullup the I2C line
    delay(1); 
    color_sensor.enableWithoutDelay(); //This reading takes the longest so queue first, read last
    light_sensor.enable(); //Start light sensor recording
    temp_sensor.takeForcedMeasurementWithoutDelay(); //Start temp sensor recording
    digitalWriteFast(I2C_pullup_pin, LOW); //Power down I2C line
    
    //Add date and time to log
    RTCms();
    log_end_index += snprintf(log_internal_buffer+log_end_index, log_internal_buffer_size-log_end_index, 
    "%4d/%02d/%02d," //log date
    "%02d:%02d:%02d.%03d," //log time
    , year(unix_t), month(unix_t), day(unix_t), //Get date
    hour(unix_t), minute(unix_t), second(unix_t), unix_ms); //Get time
    
    
    //Enter low power state while waiting for sensors to get recordings
    if(color_integration_value[color_integration_index] > light_integration_value[light_integration_index]) timer.setTimer((uint16_t) (color_integration_value[color_integration_index] + 1));
    else timer.setTimer((uint16_t) (light_integration_value[light_integration_index] + 1)); 
    sensorDelay();
    
    //Retrieve sensor data
    digitalWriteFast(I2C_pullup_pin, HIGH); //Pullup the I2C line
    delay(1);
    Vbat = checkVbat();
    Vin = checkVin();    
    temp = temp_sensor.readTemperature(); //Read temp first as it is needed to calibrate pressure and hum
    pres = (temp_sensor.readPressure() / 100.0F);
    hum = temp_sensor.readHumidity(); 
    lum = light_sensor.getFullLuminosity();
    full = (uint16_t) (lum & 0xFFFF);
    IR = (uint16_t) (lum >> 16);
    response = autoGain('l', full); //Adjust gain if necessary
    color_sensor.getRawDataWithoutDelay(&r, &g, &b, &c);
    response = autoGain('c', c); //Adjust gain if necessary
    color_sensor.disable(); //Put color sensor in low power state
    light_sensor.disable(); //Put light sensor in low power state
    digitalWriteFast(I2C_pullup_pin, LOW); //Power down I2C line
  
    //Print sensor data to log buffer
    log_end_index += snprintf(log_internal_buffer+log_end_index, log_internal_buffer_size-log_end_index, 
    "%.2f,%.2f,%.2f," //Temp, pressure, and humidity
    "%.6g,%.6g,%d,%d,%d," //Light: Full Gain, IR Gain, Integration, Full ADC, IR ADC
    "%.6g,%d,%d,%d,%d,%d," //Color: Gain, Integration, Red ADC, Green ADC, Blue ADC, Clear ADC
    "%.3f,%.3f,%c" //Battery: Vin, Vbat and print comment
    , temp, pres, hum, //Temp sensor
    vis_gain_value[light_gain_index], IR_gain_value[light_gain_index], light_integration_value[light_integration_index], full, IR, //Light sensor
    color_gain_value[color_gain_index], color_integration_value[color_integration_index], r, g, b, c, //Light sensor 
    Vin, Vbat, response); //Battry values
    log_internal_buffer[log_end_index++] = '\n'; //Add newline character to end of log line
    
    //End of log
    log_start_index = saveToSD(current_log_file_path, (char *) log_internal_buffer, log_start_index, log_end_index, false);
  }
  if((log_internal_buffer+log_end_index) > log_reset_pointer){ //If the buffer is about to overflow, reset the buffer
    uint32_t log_remainder_size = log_end_index-log_start_index;
    memcpy(log_internal_buffer, log_internal_buffer+log_start_index, log_remainder_size); //Copy the unsaved remainder of the buffer to the start of the buffer
    log_start_index = 0; //Reset starting pointer to start of buffer
    log_end_index = log_start_index + log_remainder_size; //Reset ending pointer to end of unsaved buffer
  }
}

void initializeLog(){
  //Initialize logging variables
  log_line_count = 1;
    
  //Sync log timing to button press
  next_log_time = unix_t;

  //Add header to log queue
  log_end_index += snprintf(log_internal_buffer+log_end_index, log_internal_buffer_size-log_end_index, log_header);
  log_internal_buffer[log_end_index++] = '\n'; //Add newline character to end of header 
  logEvent();
}

//Use the up and down buttons to vertically scroll through the current window
void scrollWindow(uint8_t src){
  char disp_line[LCD_dim_x];
  if(LCD_line_index > LCD_window_lines[LCD_window_index]-4) LCD_line_index = LCD_window_lines[LCD_window_index]-4; //Reset LCD line index if it is out of bounds
  if(LCD_window_index >= n_windows) LCD_window_index = n_windows-1; //Reset LCD window index if it is out of bounds
  if(src == joystick_pins[0]){ //If up press, scroll window up
    if(!LCD_line_index) return; //If already at the top, do nothing
    else LCD_line_index--;
  }
  else if(src == joystick_pins[2]){ //If down press, scroll window down
    if(LCD_line_index == LCD_window_lines[LCD_window_index]-4) return; //If already at the bottom, do nothing
    else LCD_line_index++; 
  }
  else; //Otherwise, don't change anything and reprint window
    
  //Retrieve and print corresponding strings
  for(int a=LCD_line_index; a<LCD_line_index+4; a++){
    for(int b=0; b<LCD_dim_x; b++){
      disp_line[b] = *((LCD_windows[LCD_window_index] + a*LCD_dim_x) + b);
    }
    disp_line[LCD_dim_x-1] = 0; //Force termination at end of string - prevent overlfow issues
    lcd.setCursor(0,a-LCD_line_index);
    lcd.println(disp_line);  
  }
  //Add scroll arrows if window continues past edge
  if(LCD_line_index){
    lcd.setCursor(LCD_dim_x-2, 0);
    lcd.write(0);
  }
  if(LCD_line_index < LCD_window_lines[LCD_window_index]-4){
    lcd.setCursor(LCD_dim_x-2, 3);
    lcd.write(1);
  }
}

//Use the left and right buttons to horizontally switch windows
void cycleWindow(uint8_t src){
  char disp_char;
  LCD_line_index = 0; //Reset the LCD line index to top of window
  if(LCD_window_index >= n_windows) LCD_window_index = n_windows-1; //Reset LCD window index if it is out of bounds
  if(src == joystick_pins[1]){ //If right press, cycle window right
    if(++LCD_window_index >= n_windows) LCD_window_index = 0; //Loop window index around when reach end
    for(int a=0; a<LCD_dim_x-1; a++){
      lcd.scrollDisplayLeft();
      for(int b = 0; b<4; b++){
        disp_char = *((LCD_windows[LCD_window_index] + b*LCD_dim_x) + a);
        lcd.setCursor(LCD_dim_x-2, b);
        lcd.print(disp_char);
      }
      delay(50);
    }
  }
  else{ //If left press, cycle window left
    if(LCD_window_index == 0) LCD_window_index = n_windows-1; //Loop window index around when reach end
    else LCD_window_index--;
    for(int a=LCD_dim_x-2; a>=0; a--){
      lcd.scrollDisplayRight();
      for(int b = 0; b<4; b++){
        disp_char = *((LCD_windows[LCD_window_index] + b*LCD_dim_x) + a);
        lcd.setCursor(0, b);
        lcd.print(disp_char);
      }
      delay(50);
    }
  }
  lcd.setCursor(LCD_dim_x-2, 3);
  lcd.write(1);

  //Turn on cursor for settings window
  if(LCD_window_index == n_windows-1){
    lcd.setCursor(0, 0);
    lcd.cursor();
  }
  else{
    lcd.noCursor();
  }
}

//Check all components and write boot log
void initializeDevice(){
  log_active = false;
  boot_index = 0;
  strcpy(boot_disp[boot_index++], "Boot status:        ");

  //Disable built-in LED and LCD backlight on boot
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWriteFast(LED_BUILTIN, LOW);
  pinMode(LED_PWM_pin, OUTPUT); //Turn backlight control on
  digitalWrite(LED_PWM_pin, LOW); //Turn of backlight at start of boot

  //Initialize joystick pins - map to digital wake from hibernate
  for(int a = 0; a<5; a++){
    digital.pinMode(joystick_pins[a], INPUT_PULLUP, FALLING);
  }
  
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
  pinMode(Vin_test_pin, OUTPUT);
  digitalWriteFast(Vin_test_pin, LOW); //Disconnect battery from ADC
  if(measure_battery){
    pinMode(coin_analog_pin, INPUT); //Set coin cell ADC to floating high impedance
    pinMode(Vin_analog_pin, INPUT); //Set battery ADC to floating high impedance
  }
  
  //Synchronize Teensy clock to computer clock
  setSyncProvider(getTeensy3Time); //Sync to computer clock - do not use () in function as argument
  if (timeStatus()!= timeSet) { //If sync failed
    strcpy(boot_disp[boot_index++], "RTC time sync fail! ");
    warning_count += 1;
  } else { // if Sync successful
    strcpy(boot_disp[boot_index++], "RTC sync successful ");
  }
  RTCms(); //Get current time
  next_log_time = unix_t + log_interval;
  setHibernateTimer();
  sprintf(boot_disp[boot_index++], "%4d/%02d/%02d %02d:%02d:%02d ", year(unix_t), month(unix_t), day(unix_t), hour(unix_t), minute(unix_t), second(unix_t)); //Write current time to boot screen
  
  //Setup I2C communication to highest speed chips sill support - sensor libraries will initialize I2C communications
  Wire.setClock(400000); 
  Wire1.setClock(400000);

  //Start LCD display
  analogWriteResolution(analog_resolution); //Set DAC and PWM resolution - NOTE: Do not change once set!
  analogReadResolution(analog_resolution); //Set ADC resolution - NOTE: Do not change once set!
  analogWriteFrequency(LED_PWM_pin, analog_freq); //Set LED PWM freq - other pins on same timer will also change - https://www.pjrc.com/teensy/td_pulse.html
  pinMode(LCD_toggle_pin, OUTPUT); //Turn display on
  pinMode(LED_PWM_pin, OUTPUT); //Turn backlight control on
  digitalWrite(LCD_toggle_pin, HIGH);
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
    disableDisplay(true);
    display_present = false;
    display_on = false;
    backlight_on = false;
  }
  lcd.setCursor(0,0);
  lcd.println("Booting logger...");
  
  //Initialize sensors
  if(temp_sensor.begin(0x77, temp_port)){
    strcpy(boot_disp[boot_index++], "Temp sensor....OK   ");
    temp_present = true;
    temp_on = true;

    //Set sensor to run in forced mode without filtering or oversampling
    temp_sensor.setSampling(Adafruit_BME280::MODE_FORCED,
                Adafruit_BME280::SAMPLING_X1, // temperature
                Adafruit_BME280::SAMPLING_X1, // pressure
                Adafruit_BME280::SAMPLING_X1, // humidity
                Adafruit_BME280::FILTER_OFF   );
  }
  else{
    strcpy(boot_disp[boot_index++], "Temp sensor....FAIL!");
    warning_count += 1;
    digitalWriteFast(temp_power_pin, LOW); //Cut power to sensor
    temp_present = false;
    temp_on = false;
  }
  lcd.setCursor(0,1);
  lcd.println("Setting color sensor");
  if(color_sensor.begin(0x29, color_port)){
    uint16_t test_channel, dummy_channel;
    strcpy(boot_disp[boot_index++], "Color sensor...OK   ");
    color_present = true;
    color_on = true;

    //Set gain and integration time
    color_sensor.enable();
    color_sensor.setGain(color_gain_command[color_gain_index]);
    color_sensor.setIntegrationTime(color_integration_command[color_integration_index]);
    timer.setTimer((uint16_t) (color_integration_value[color_integration_index] + 1));
    sensorDelay();
    color_sensor.getRawDataWithoutDelay(&dummy_channel, &dummy_channel, &dummy_channel, &test_channel);
    sensorDelay();
    color_sensor.getRawDataWithoutDelay(&dummy_channel, &dummy_channel, &dummy_channel, &test_channel);
    autoGain('c', test_channel); 
    color_sensor.disable();  
  }
  else{
    strcpy(boot_disp[boot_index++], "Color sensor...FAIL!");
    warning_count += 1;
    digitalWriteFast(color_power_pin, LOW); //Cut power to sensor
    color_present = false;
    color_on = false;
  }
  lcd.setCursor(0,2);
  lcd.println("Setting light sensor");
  if(light_sensor.begin(light_port)){
    uint16_t test_channel;
    uint32_t lum;
    strcpy(boot_disp[boot_index++], "Light sensor...OK   ");
    light_present = true;
    light_on = true;

    //Set gain and integration time
    light_sensor.enable();
    light_sensor.setGain(light_gain_command[light_gain_index]);
    light_sensor.setTiming(light_integration_command[light_integration_index]);
    timer.setTimer((uint16_t) (light_integration_value[light_integration_index] + 1));
    sensorDelay();
    lum = light_sensor.getFullLuminosity();
    test_channel = lum & 0xFFFF;
    autoGain('l', test_channel); 
    light_sensor.disable();   
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
  }
  if(volt < 2.8){
    strcpy(boot_disp[boot_index++], "Vbat low voltage!   ");
    warning_count += 1;
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
  lcd.setCursor(0,3);
  lcd.print("Initialize SD");
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
  
  //Search for the most recent log file and increment log_file_count to next file ID
  if(SD.exists(log_dir)){
    for(log_file_count = 0; log_file_count < max_log_file-1; log_file_count++){
      sprintf(current_log_file_path, "%s/%02d%02d%02d%c%c.csv", log_dir, year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count/26)+97, (log_file_count%26)+97);     
      if(!SD.exists(current_log_file_path)) break;
    }
    sprintf(current_boot_file_path, "%s/%02d%02d%02d%c%c.txt", boot_dir, year(unix_t)-2000, month(unix_t), day(unix_t), (log_file_count/26)+97, (log_file_count%26)+97); 
  }
 
  //Report number of warning encountered
  if(warning_count){
    sprintf(boot_disp[boot_index++], "% 2d Warnings on boot!", warning_count);
  }
  else{
    strcpy(boot_disp[boot_index++], "Success! 0 Warnings ");
  }
  
  //Save current boot log
  for(int a = 0; a<boot_index; a++){
    boot_disp[a][LCD_dim_x-1] = '\n'; //Replace all nulls with new lines for *.txt formatting
  }
  if(saveToSD(current_boot_file_path, (char *) boot_disp, 0, boot_index*LCD_dim_x, true) != boot_index*LCD_dim_x){
    strcpy(boot_disp[boot_index++], "Boot save FAIL!     ");
  }
  for(int a = 0; a<boot_index; a++){
    boot_disp[a][LCD_dim_x-1] = 0; //Replace all new lines with nulls for println() formatting
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
    }
    lcd.setCursor(LCD_dim_x-2, 0);
    lcd.write(0);
  }
}

//Automatically adjust gain on light sensors
char autoGain(char sensor, uint16_t test_channel){
  uint32_t lum; //Light sensor returns to channels as a concatenated 32-bit value that needs to be split
  uint16_t dummy_channel;
  uint16_t *over_exposed, *under_exposed;
  uint8_t counter, start_counter, max_index;
  uint8_t *gain_index, *integration_index, *gain_command, *integration_command;
  RTCms(); //Get current time
  

  //Get sensor settings
  if(sensor == 'c'){
    counter = 2*(color_max_index+1)+1; //Sanity check to stop loop after all possible changes have been made
    start_counter = counter; 
    gain_index = &color_gain_index;
    integration_index = &color_integration_index;
    over_exposed = &color_over_exposed;
    under_exposed = &color_under_exposed;
    max_index = color_max_index;
    gain_command = color_gain_command;
    integration_command = color_integration_command;   
  }
  else{
    counter = 2*(light_max_index+1)+1;
    start_counter = counter; 
    gain_index = &light_gain_index;
    integration_index = &light_integration_index;
    over_exposed = &light_over_exposed;
    under_exposed = &light_under_exposed;
    max_index = light_max_index;
    gain_command = light_gain_command;
    integration_command = light_integration_command;
  }
   
  //Adjust the gain as if reading is out of range as long as exposure can still be adjusted - timeout at log interval if logging 
  do{ //Use do/while to ensure the value is checked at least once before adjusting the gain
    //Adjust exposure index based on sensor reading if there is still adjustment range left
    if(test_channel > *over_exposed){
      if(*gain_index) --*gain_index;
      else if(*integration_index) --*integration_index;
      else return 's'; //Otherwise, tell calling function that the sensor is saturated
    }
    else if(test_channel < *under_exposed){
      if(*integration_index < max_index) ++*integration_index;
      else if(*gain_index < max_index) ++*gain_index;
      else return 'm'; //Otherwise, tell calling function that the sensor is already at max gain and integration time;
    }
    else{ //If no change needs to be made, return 0
      if(counter == start_counter) return 'z';
    }
    
    //Get sensor readings and update over and under exposure values
    if(sensor == 'c'){
      color_sensor.setGain(color_gain_command[*gain_index]);
      color_sensor.setIntegrationTime(color_integration_command[*integration_index]);
      timer.setTimer((uint16_t) (color_integration_value[*integration_index] + 1));
      sensorDelay();
      color_sensor.getRawDataWithoutDelay(&dummy_channel, &dummy_channel, &dummy_channel, &test_channel);
      sensorDelay();
      color_sensor.getRawDataWithoutDelay(&dummy_channel, &dummy_channel, &dummy_channel, &test_channel);
      *over_exposed = color_integration_max_count[color_integration_index] >> color_over_shift;
      *under_exposed = color_integration_max_count[color_integration_index] >> color_under_shift;  //Exposure steps in 2^4 so min is 1 + 2^(4) + 1 = >>6  
    }
    else{
      light_sensor.setGain(light_gain_command[*gain_index]);
      light_sensor.setTiming(light_integration_command[*integration_index]);
      timer.setTimer((uint16_t) (light_integration_value[*integration_index] + 1));
      sensorDelay();
      lum = light_sensor.getFullLuminosity();
      test_channel = lum & 0xFFFF;
      *over_exposed = light_integration_max_count[light_integration_index] >> light_over_shift;
      *under_exposed = light_integration_max_count[light_integration_index] >> light_under_shift; //Gain steps in 2^5 so min is 1 + 2^(5) + 1 = >>7
    }
    RTCms(); //Update the clock
  } while(counter-- && (unix_t < next_log_time-1 || !log_active) && (test_channel > *over_exposed && *integration_index) || (test_channel < *under_exposed  && *gain_index < max_index));
  
  return 'c'; //Tell calling function that exposure was changed 
}

//Hibernate while sensors take reading
void sensorDelay(){
  wakeup_source = Snooze.hibernate(hibernate_config); //Enter low power state while waiting for sensors to record data
  if(wakeup_source > 33) wakeup_source = 0; //Reset wakeup flag for timer wake
  else delay(700); //Otherwise, button was pressed so wait for rest of sensor read, and then process press
}

//Sync the global unix_t and unix_ms - Get RTC with ms precision - code from: https://github.com/manitou48/teensy3/blob/master/RTCms.ino
void RTCms(){
  uint32_t read1, read2, us;
  do{
        read1 = RTC_TSR;
        read2 = RTC_TSR;
    }while( read1 != read2);       //insure the same read twice to avoid 'glitches'
    unix_t = read1;
    do{
        read1 = RTC_TPR;
        read2 = RTC_TPR;
    }while( read1 != read2);       //insure the same read twice to avoid 'glitches'
  //Scale 32.768KHz to microseconds, but do the math within 32bits by leaving out 2^6
  // 30.51758us per TPR count
  us = (read1*(1000000UL/64)+16384/64)/(32768/64);    //Round crystal counts to microseconds
  if( us < 100 ) //if prescaler just rolled over from zero, might have just incremented seconds -- refetch
  {
    do{
        read1 = RTC_TSR;
        read2 = RTC_TSR;
    }while( read1 != read2);       //insure the same read twice to avoid 'glitches'
    unix_t = read1; //Update the unix_t global variable
  }
  unix_ms = us/1000;
}

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

//Check coin cell voltage - since the ADC for the coin cell down is floating, we have to do a PULLDOWN to a known state first to see if a battery is connected before trying to measure the voltage
float checkVbat(){
  float coin_voltage = 0;
  pinMode(coin_analog_pin, INPUT_PULLDOWN);
  if(digitalRead(coin_analog_pin)){
    pinMode(coin_analog_pin, INPUT);
    coin_voltage = analogRead(coin_analog_pin);
    coin_voltage *= 3.3/analog_max;
  }
  pinMode(coin_analog_pin, INPUT_DISABLE); //Return input to high impedance state
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
  pinMode(Vin_analog_pin, INPUT_DISABLE); //Return input to high impedance state
  return Vin_voltage;
}

//Save data to the SD card
uint32_t saveToSD(char (*file_path), char *data_array, uint32_t start_index, uint32_t end_index, boolean force_write){
  uint32_t log_size = end_index-start_index;
  if(save_on_log_interval) force_write = true; //Save buffer every cycle if save flag is set
  
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print((uint16_t) start_index);
//  lcd.setCursor(0,1);
//  lcd.print((uint16_t) end_index);
//  lcd.setCursor(0,2);
//  lcd.print((uint16_t) log_line_count);
  
  if(force_write || log_size >= 512){ 
    if(SD.exists(log_dir)){ //Make sure that the save directories exist before trying to save to it - without this check open() will lock without SD card  

      //Open file
      f = SD.open(file_path, FILE_WRITE);
      if(f){
        while(log_size >= 512){ //Retrieve a blocks of 512 bytes
          f.write((data_array + start_index), 512);
          start_index += 512;
          log_size = end_index-start_index; //Update remaining log size
        }
        if(force_write){ //Print remainder of data buffer if force_write is enabled
          f.write((data_array + start_index), log_size);
          start_index += log_size;                       
        }
        f.close(); //File timestamp applied on close (save)
      }
    }
    else noSdHold();
  }
  return start_index;
}

void noSdHold(){
  disableDisplay(false);
  lcd.createChar(1, down_arrow);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please insert an SD");
  lcd.setCursor(0,1);
  lcd.print("card formatted to  ");
  lcd.setCursor(0,2);
  lcd.print("FAT32 and reboot  ");
  lcd.setCursor(0,3);
  for(uint8_t a = 0; a<7; a++){
    lcd.write(1);
  }
  timer.setTimer(65535);
  wakeup_source = 34;
  while(wakeup_source > 33) wakeup_source = Snooze.hibernate(hibernate_config);
  WRITE_RESTART(0x5FA0004);
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
void disableDisplay(boolean disable){
  if(disable){
    digitalWriteFast(LCD_toggle_pin, LOW); //Turn off LCD power - Do NOT turn off LCD if it is connected as this draws more power - input pins in low impedance?
    pinMode(LCD_toggle_pin, INPUT_DISABLE);
    lcd.noDisplay(); //Turn off display by command
    digitalWriteFast(LED_PWM_pin, LOW); //Turn off backlight
    analogWrite(contrast_pin, 0); //Turn off LCD contrast
    DAC0_C0 = 0; //Disable DAC pin DAC0 to save power on hibernate - https://github.com/duff2013/Snooze/issues/12 - unsigned char to fix warning - https://www.avrfreaks.net/forum/warning-large-integer-implicitly-truncated-unsigned-type
    for(int a=0; a<(sizeof(LCD_pin)/sizeof(LCD_pin[0])); a++) pinMode(LCD_pin[a], INPUT_DISABLE); //Set digital pins to high impedance
    display_on = false;
  }
  else{ 
    for(int a=0; a<(sizeof(LCD_pin)/sizeof(LCD_pin[0])); a++) pinMode(LCD_pin[a], OUTPUT); //Set digital pins to high impedance
    DAC0_C0 = 1; //Disable DAC pin DAC0 to save power on hibernate - https://github.com/duff2013/Snooze/issues/12 - unsigned char to fix warning - https://www.avrfreaks.net/forum/warning-large-integer-implicitly-truncated-unsigned-type
    pinMode(LCD_toggle_pin, OUTPUT);
    digitalWriteFast(LCD_toggle_pin, HIGH); //Turn on LCD power
    delay(100);
    lcd.begin(20, 4); //Initialize LCD
    setLCDbacklight(default_backlight); //Turn on LED backlight to default intensity
    setLCDcontrast(default_contrast); //Initialize screen contrast to default value
    display_on = true;
  }
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

//void wakeUSB(){
//    elapsedMillis time = 0;
//    while (!Serial && time < 2000) {
//        Serial.write(0x00);// print out a bunch of NULLS to serial monitor
//        digitalWriteFast(LED_BUILTIN, HIGH);
//        delay(30);
//        digitalWriteFast(LED_BUILTIN, LOW);
//        delay(30);
//    }
//    // normal delay for Arduino Serial Monitor
//    delay(200);
//}

//************************************************************************
static void  BigNumber_SendCustomChars(void)
{
uint8_t customCharDef[10];
uint8_t ii;
int   jj;

  for (ii=0; ii<8; ii++)
  {
    for (jj=0; jj<8; jj++)
    {
      customCharDef[jj] = pgm_read_byte_near(gBigFontShapeTable + (ii * 8) + jj);
    }
    lcd.createChar(ii, customCharDef);
  }
}

//************************************************************************
//* returns the width of the character
static int  DrawBigChar(int xLocation, int yLocation, char theChar)
{
int   offset;
int   ii;
char  theByte;
boolean specialCase;
int   specialIndex;
int   charWidth;

  if (theChar == 0x20)
  {
    return(2);
  }
  else if (theChar < 0x20)
  {
    return(0);
  }

  if (theChar >= 'A')
  {
    theChar = theChar & 0x5F; //* force to upper case
  }
  specialCase = true;
  switch (theChar)
  {
    case 'M': charWidth = 5;  specialIndex  = 0;  break;
    case 'N': charWidth = 4;  specialIndex  = 1;  break;
    case 'Q': charWidth = 4;  specialIndex  = 2;  break;
    case 'V': charWidth = 4;  specialIndex  = 3;  break;
    case 'W': charWidth = 5;  specialIndex  = 4;  break;


    default:
      charWidth = 3;
      specialCase = false;
      offset    = 6 * (theChar - 0x20);
      lcd.setCursor(xLocation, yLocation);
      for (ii=0; ii<3; ii++)
      {
        theByte = pgm_read_byte_near(gBigFontAsciiTable + offset + ii);
        lcd.write(theByte);
      }

      lcd.setCursor(xLocation, yLocation + 1);
      offset  +=  3;
      for (ii=0; ii<3; ii++)
      {
        theByte = pgm_read_byte_near(gBigFontAsciiTable + offset + ii);
        lcd.write(theByte);
      }
      break;
  }
  if (specialCase)
  {
    //*
    offset    = 10 * specialIndex;
    lcd.setCursor(xLocation, yLocation);
    for (ii=0; ii<charWidth; ii++)
    {
      theByte = pgm_read_byte_near(gBigFontAsciiTableWide + offset + ii);
      lcd.write(theByte);
    }

    lcd.setCursor(xLocation, yLocation + 1);
    offset  +=  5;
    for (ii=0; ii<charWidth; ii++)
    {
      theByte = pgm_read_byte_near(gBigFontAsciiTableWide + offset + ii);
      lcd.write(theByte);
    }
  }
  return(charWidth + 1);
}
