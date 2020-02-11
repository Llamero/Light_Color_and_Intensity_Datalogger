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
uint8_t log_interval_array[] = {0, 0, 5}; //Number of hours, minutes, and seconds between log intervals
const char boot_dir[] = "boot_log"; //Directory to save boot log files into - max length 8 char
const char log_dir[] = "data_log"; //Directory to save data log files into - max length 8 char
boolean measure_temp = true;
boolean measure_humidity = true;
boolean measure_pressure = true;
boolean measure_lux = true;
boolean measure_color = true;
boolean measure_battery = true;

boolean disable_display_on_log = true; //Completely power down display during logging
const float default_backlight = 0; //Set default backlight intensity to full brightness (range is 0-1)
const float default_contrast = 0.5; //Set default LCD contrast to half range (range is 0-1)

//Allow Teensy to re-run setup when settings are changed - Call "CPU_RESTART"
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

// Load drivers
SnoozeDigital digital;
SnoozeTimer timer;
SnoozeBlock hibernate_config(digital, timer);
time_t unix_t = 0; //Track current device time 
time_t next_log_time = 0; //next log time in unix time
time_t time_remaining = 0; //Time remaing to next log interval
time_t log_interval = (60*60*log_interval_array[0])+(60*log_interval_array[1])+log_interval_array[2];
const uint16_t log_offset = 1000; //Time in ms it takes to log a data point - also determines smallest log interval - can't be >5535 ms.
boolean log_next_wake = false; //Whether to log on the next timer wake event

//Setup LCD pin numbers and initial parameters
const uint8_t LCD_pin[] = {30, 34, 35, 5, 4, 3, 1}; //Only 4-pin supported in LiquidCrystalFast
              // LCD pins: RS  RW  EN  D4 D5 D6 D7
LiquidCrystalFast lcd(LCD_pin[0], LCD_pin[1], LCD_pin[2], LCD_pin[3], LCD_pin[4], LCD_pin[5], LCD_pin[6]);
const uint8_t LCD_toggle_pin = 24; //Set to high to power on LCD
const uint8_t LED_PWM_pin = 29; //Drive LED backlight intensity
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
const uint8_t light_interrupt_pin = 33;
const uint8_t I2C_pullup_pin = 12; //Pin providing I2C pullup voltage
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Sensor settings:
const uint8_t color_gain_value[] = {1, 4, 16, 60};
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
const float light_integration_value[] = {100, 200, 400, 600};
const uint8_t light_integration_command[] = {0x00, 0x01, 0x03, 0x05};
const uint16_t light_integration_max_count[] = {36863, 65535, 65535, 65535}; //Max ADC count for given integration time 
uint8_t light_integration_index = light_max_index; //Start at maximum sensitivity
const uint8_t light_over_shift = 1; //Over-saturated cutoff - number of shifts to max value (= divide by 2 per shift)
const uint8_t light_under_shift = light_over_shift + 6; //Gain steps in 2^5 so min is 5 + 1 = >>6 
uint16_t light_over_exposed = light_integration_max_count[light_integration_index] >> light_over_shift;
uint16_t light_under_exposed = light_integration_max_count[light_integration_index] >> light_under_shift;

//Setup joystick pins
const uint8_t joystick_pins[] = {9, 11, 2, 7, 10}; //Joystick pins - up, right, down, left, push 
                              //u,  r,  d, l, center 
const uint8_t debounce = 50; //Time in ms to debounce button press

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
const char log_header[] = "Date,Time,Temperature(°C),Pressure(hPa),Humidity(%),Intensity(lux),Red(µW/cm^2),Green(µW/cm^2),Blue(µW/cm^2),Clear(µW/cm^2),Vin(V),Vbat(V),Comment,";
const uint8_t n_log_columns = 13; //Number of items to log per round
uint8_t boot_index = 0; //Index of boot message
int warning_count = 0; //Number of warnings encountered during boot 
const uint32_t internal_log_size = 100000; //Maximum number of bytes to be stored internally
volatile char internal_log_backup[internal_log_size]; //Array for storing data logs while SD card is not available 
uint32_t log_internal_index = 0; //Index of internal log string
uint32_t log_internal_count = 0; //Number of stings that have been added to internal log
uint16_t log_file_index = 0; //Index of current line in log file
char current_file_name[13]; //Storing the current log file name - max length for FAT32 is 8.3 - 13 characters total including null at end
uint16_t log_file_count = 0; //Counter for tracking number of log files in ascii (track files from aa to zz)
const uint16_t max_log_file = 26*26; //Largest number of log files available on the same date
boolean log_active = false; //Whether the device is actively logging or in standby state

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, color_gain_command[color_gain_index]); //Create instance of color sensor initialize with peak sensitivity
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
uint32_t counter = 0;

unsigned long delayTime;

/**************************************************************************/
/*
    Configures the gain and integration time for the tsl2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //light_sensor.setGain(tsl2591_GAIN_LOW);    // 1x gain (bright light)
  //light_sensor.setGain(tsl2591_GAIN_MED);      // 25x gain
  light_sensor.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //light_sensor.setTiming(tsl2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // light_sensor.setTiming(tsl2591_INTEGRATIONTIME_200MS);
  //light_sensor.setTiming(tsl2591_INTEGRATIONTIME_300MS);
  // light_sensor.setTiming(tsl2591_INTEGRATIONTIME_400MS);
  // light_sensor.setTiming(tsl2591_INTEGRATIONTIME_500MS);
   light_sensor.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = light_sensor.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((light_sensor.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

void setup() {
    //Power on the sensors
    pinMode(temp_power_pin, OUTPUT);
    pinMode(color_power_pin, OUTPUT);
    pinMode(light_power_pin, OUTPUT);
    pinMode(I2C_pullup_pin, OUTPUT);
    digitalWriteFast(temp_power_pin, (measure_temp || measure_humidity || measure_pressure)); //Power temp_sensor280 if any evironmental variable is to be sensed
    digitalWriteFast(color_power_pin, measure_color);
    digitalWriteFast(light_power_pin, measure_lux);
    digitalWriteFast(I2C_pullup_pin, (measure_temp || measure_humidity || measure_pressure || measure_lux || measure_color));
    
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("temp_sensor280 test"));

    unsigned status;
    
    // default settings
    status = temp_sensor.begin(0x77, temp_port);  
    // You can also pass in a Wire library object like &Wire2
    // status = temp_sensor.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid temp_sensor280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(temp_sensor.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a temp_sensor 280.\n");
        Serial.print("        ID of 0x61 represents a temp_sensor 680.\n");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();
    if (color_sensor.begin(0x29, color_port)) {
      Serial.println("Found sensor");
    } else {
      Serial.println("No color_sensor34725 found ... check your connections");
      while (1);
    }
    if (light_sensor.begin(light_port)) 
    {
    Serial.println(F("Found a tsl2591 sensor"));
    } 
    else 
    {
      Serial.println(F("No sensor found ... check your wiring?"));
      while (1);
    }
    /* Configure the sensor */
    configureSensor();
}


void loop() { 
    printValues();
    delay(delayTime);
    uint16_t r, g, b, c, colorTemp, lux;

    color_sensor.getRawData(&r, &g, &b, &c);
    // colorTemp = color_sensor.calculateColorTemperature(r, g, b);
    colorTemp = color_sensor.calculateColorTemperature_dn40(r, g, b, c);
    lux = color_sensor.calculateLux(r, g, b);
  
    Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
    Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
    Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
    Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
    Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
    Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
    Serial.println(" ");
    advancedRead();
}


void printValues() {
    Serial.print("Temperature = ");
    Serial.print(temp_sensor.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(temp_sensor.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(temp_sensor.readHumidity());
    Serial.println(" %");

    Serial.println();
}

void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = light_sensor.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(light_sensor.calculateLux(full, ir), 6);
}
