#include "LCD.h"

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

LCD lcd(DB_pin_array, n_DB_pin, RS_pin, RW_pin, E_pin, LCD_toggle_pin, LED_PWM_pin, contrast_pin);
float inc = 0;
float contrast = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  analogWriteResolution(12);
  wakeUSB();
  lcd.initializeLCD();
}

void loop() {
  contrast = sin(inc)*0.5 + 0.5;
  lcd.setLCDcontrast(contrast);
  inc = inc + 0.02;
  if(inc>2*PI) inc = 0;
  //delayMicroseconds(10);
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
