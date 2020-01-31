#include "Adafruit_BME280.h" //Temp, pressure, humidity sensor
#include "Adafruit_Sensor.h" //Universal sensor API
#include "Adafruit_TCS34725.h" //Color Sensor
#include "Adafruit_TSL2591.h" //Light Sensor
#include "LCD.h" //Display
#include <Wire.h> //I2C
#include <TimeLib.h> //Set RTC time and get time strings
#include <Snooze.h> //Put Teensy into low power state between log points

//Setup LCD pin numbers and initial parameters
//int DB_pin_array[] = {32, 31, 8, 6, 5, 4, 3, 1}; //List of DB0-DB7 pins to send data to LCD - 4-pin is not fully supported
int DB_pin_array[] = {5, 4, 3, 1}; //List of DB0-DB7 pins to send data to LCD - 4-pin is not fully supported
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
TwoWire* temp_port = &Wire; //Set I2C (wire) ports
TwoWire* color_port = &Wire;
TwoWire* light_port = &Wire1;

//Setup battery test pin numbers
int coin_test_pin = 36;
int coin_analog_pin = A1;
int Vin_test_pin = 39;
int Vin_analog_pin = A0; 

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


//Arrays for storing display Strings
char boot_array[12][20]; //Array for boot display

//Initialize libraries
Adafruit_BME280 temp_sensor; //Create instance of temp sensor
Adafruit_TCS34725 color_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_60X); //Create instance of color sensor initialize with peak sensitivity
Adafruit_TSL2591 light_sensor = Adafruit_TSL2591(2591); //Create instance of light sensor - number is sensor ID
LCD lcd(DB_pin_array, n_DB_pin, RS_pin, RW_pin, E_pin, LCD_toggle_pin, LED_PWM_pin, contrast_pin, analog_resolution); //Create instance of LCD display

time_t t = 0;
float inc = 0;
float contrast = 0;
void setup() {
  int boot_index = 0;
  strcpy(boot_array[boot_index++], "Boot status:        ");

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
    strcpy(boot_array[boot_index++], "Display not found   ");
    display_present = false;
    display_on = false;
    backlight_on = false;
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

  //Initialize sensors
  if(temp_sensor.begin(0x77, temp_port)){
    strcpy(boot_array[boot_index++], "Temp sensor....OK   ");
    temp_present = true;
    temp_on = true;
  }
  else{
    strcpy(boot_array[boot_index++], "Temp sensor....FAIL ");
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
    strcpy(boot_array[boot_index++], "Color sensor...FAIL ");
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
    strcpy(boot_array[boot_index++], "Light sensor...FAIL ");
    digitalWriteFast(light_power_pin, LOW); //Cut power to sensor
    light_present = false;
    light_on = false;  
  }

  //Test batteries
  float volt = checkCoinCell();
  
  
  while(!Serial);
  for(int a = 0; a<boot_index; a++){
    for(int b=0; b<20; b++){
      Serial.print(boot_array[a][b]);
    }
    Serial.println();
  }
  for(int a = 0; a<boot_index-3; a++){
    lcd.displayCharArray(boot_array, 0, a+1, a+2, a+3);
    delay(1000);
  }

}

void loop() {
  //contrast = sin(inc)*0.5 + 0.5;
  //lcd.setLCDcontrast(contrast);
  //inc = inc + 0.02;
  //if(inc>2*PI) inc = 0;
  //light_sensor.begin(light_port); //Specify I2C port when initializaing library
  if(Serial.available()){
    delay(100);
    float contrast = Serial.parseFloat();
    lcd.setLCDcontrast(contrast);
    while(Serial.available()){
      Serial.read();
    }    
  }
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
float checkCoinCell(){
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
float checkBattery(){
  digitalWriteFast(Vin_test_pin, HIGH); //Connect Vin cell to ADC
  float Vin_voltage = analogRead(Vin_analog_pin);
  digitalWriteFast(Vin_test_pin, LOW); //disconnect Vin cell from ADC
  Vin_voltage = Vin_voltage/(1<<analog_resolution)*6.6;
  return Vin_voltage;
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
  t[a] = ' ';  //Issue with ' ' character rendering at end of line so other blank character used instead
}

int addLeadingZero(char *string, int num, int i){
  char buf[2];
  sprintf(buf, "%02d", num);
  string[i++] = buf[0];
  string[i++] = buf[1];
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
