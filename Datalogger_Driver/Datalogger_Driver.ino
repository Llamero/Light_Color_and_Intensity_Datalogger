#include "Adafruit_BME280.h" //Temp, pressure, humidity sensor
#include "Adafruit_Sensor.h" //Universal sensor API
#include "Adafruit_TCS34725.h" //Color Sensor
#include "Adafruit_TSL2591.h" //Light Sensor
#include "LCD.h" //Display
#include <Wire.h> //I2C
#include <TimeLib.h> //Set RTC time and get time strings
#include <Snooze.h> //Put Teensy into low power state between log points

//Setup LCD pin numbers
const int DB_pin_array[] = {32, 31, 8, 6, 5, 4, 3, 1}; //List of 4 (DB4-DB7) or 8 (DB0-DB7) pins to send data to LCD
//const int DB_pin_array[] = {5, 4, 3, 1}; //Uncomment for 4 pin operation
const int RS_pin = 30;
const int RW_pin = 34;
const int E_pin = 35;
const int LCD_toggle_pin = 24; //Set to high to power on LCD
const int LED_PWM_pin = 29; //Drive LED backlight intensity
const int contrast_pin = A21; //DAC pin for addjusting diplay contrast
const int n_DB_pin = sizeof(DB_pin_array)/sizeof(DB_pin_array[0]);

//Setup sensor pin numbers
const int temp_power_pin = 20; //Set Vcc pins
const int color_power_pin = 23;
const int light_power_pin = 17;
const int color_interrupt_pin = 16; //Set interrupt pins
const int light_interrupt_pin = 33;
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Arrays for storing display Strings
char boot_array[10][20]; //Array for boot display

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_60X); //Create instance of color sensor initialize with peak sensitivity
Adafruit_TSL2591 light_sensor = Adafruit_TSL2591(2591); //Create instance of light sensor - number is sensor ID
LCD lcd(DB_pin_array, n_DB_pin, RS_pin, RW_pin, E_pin, LCD_toggle_pin, LED_PWM_pin, contrast_pin); //Create instance of LCD display

time_t t = 0;
float inc = 0;
float contrast = 0;
void setup() {
  int boot_index = 0;
  strcpy(boot_array[boot_index++], "Boot status:       ");

  //Turn off Teensy LED
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWriteFast(LED_BUILTIN, LOW);
  
  //Power on the sensors
  pinMode(temp_power_pin, OUTPUT);
  pinMode(color_power_pin, OUTPUT);
  pinMode(light_power_pin, OUTPUT);
  digitalWriteFast(temp_power_pin, HIGH);
  digitalWriteFast(color_power_pin, HIGH);
  digitalWriteFast(light_power_pin, HIGH);
    
  //Setup USB serial communication
  Serial.begin(9600); //Baud rate is ignored and negotiated with computer for max speed
  wakeUSB();
  
  //Setup I2C communication to highest speed chips sill support - sensor libraries will initialize I2C communications
  Wire.setClock(400000); 
  Wire1.setClock(400000);

  //Start LCD display
  analogWriteResolution(16); //Set DAC and PWM resolution
  if(lcd.initializeLCD()){ //Start LCD display and confirm if present
     strcpy(boot_array[boot_index++], "Display initialized ");
  }
  else{
    strcpy(boot_array[boot_index++], "Display not found   ");
  }

  //Synchronize Teensy clock to computer clock
  setSyncProvider(getTeensy3Time); //Sync to computer clock - do not use () in function as argument
  if (timeStatus()!= timeSet) { //If sync failed
    strcpy(boot_array[boot_index++], "RTC time sync fail  ");
  } else { // if Sync successful
    strcpy(boot_array[boot_index++], "RTC sync successful ");
  }
  t = now();
  timeString(t, boot_array[boot_index++]); //Write current time to boot screen

  while(!Serial);
  for(int a = 0; a<boot_index; a++){
    for(int b=0; b<20; b++){
      Serial.print(boot_array[a][b]);
    }
    Serial.println();
  }

}

void loop() {
  contrast = sin(inc)*0.5 + 0.5;
  lcd.setLCDcontrast(contrast);
  inc = inc + 0.02;
  if(inc>2*PI) inc = 0;
  light_sensor.begin(light_port); //Specify I2C port when initializaing library
  digitalWriteFast(LED_BUILTIN, HIGH);
  delayMicroseconds(100);
  digitalWriteFast(LED_BUILTIN, LOW);
  delayMicroseconds(100);
}

time_t getTeensy3Time(){
  return Teensy3Clock.get();
}

//Create a time string formated as "year month day hour:minute:second"
void timeString(time_t unix_t, char (*t)){ //Syndax for array in 2D array - https://stackoverflow.com/questions/16486276/c-pointer-declaration-for-pointing-to-a-row-of-2-d-array
  char buf[4];
  int a; 
  sprintf(buf, "%d", year(unix_t));
  for(a=0; a<4; a++){
    t[a] = buf[a];
  }
  t[a++] = ' ';
  a = addLeadingZero(t, month(unix_t), a);
  t[a++] = ' ';
  a = addLeadingZero(t, day(unix_t), a);
  t[a++] = ' ';
  a = addLeadingZero(t, hour(unix_t), a);
  t[a++] = ':';
  a = addLeadingZero(t, minute(unix_t), a);
  t[a++] = ':';
  a = addLeadingZero(t, second(unix_t), a);
}

int addLeadingZero(char *string, int num, int i){
  char buf[2];
  sprintf(buf, "%d", num);
  if(num < 10){
    string[i++] = '0';
    string[i++] = buf[0];
  }
  else{
    string[i++] = buf[0];
    string[i++] = buf[1];
  }
  return i;
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
