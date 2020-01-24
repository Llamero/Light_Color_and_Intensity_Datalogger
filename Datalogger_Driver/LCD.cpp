/*
  NHD-0420H1Z-FL-GBW-33V3.cpp - Library for the Newhaven Display NHD-0420H1Z-FL-GBW-33V3
*/

#include "Arduino.h"
#include "LCD.h"

LCD::LCD(int *DB_pin_array, int DB_length, int RS_pin, int RW_pin, int E_pin, int LCD_toggle_pin, int LED_PWM_pin, int contrast_pin){
  _DB_pin_array = DB_pin_array;
  _RS_pin = RS_pin;
  _RW_pin = RW_pin;
  _E_pin = E_pin;
  _DB_length = DB_length;
  _LCD_toggle_pin = LCD_toggle_pin;
  _LED_PWM_pin = LED_PWM_pin;
  _contrast_pin = contrast_pin;
}

//PUBLIC---------------------------------------------------------------------------------------------------------------------------------------------
//Start display from power on - http://www.newhavendisplay.com/specs/NHD-0420H1Z-FL-GBW-33V3.pdf
boolean LCD::initializeLCD(){
  outputPins();
  digitalWriteFast(_LCD_toggle_pin, HIGH); //Turn on LCD
  digitalWriteFast(_E_pin, LOW);
  delay(100); //Wait >40 msec after power is applied
  commandLCD(0x30); //command 0x30 = Wake up
  delay(30); //must wait 5ms, busy flag not available
  commandLCD(0x30); //command 0x30 = Wake up #2
  delay(10); //must wait 160us, busy flag not available
  commandLCD(0x30); //command 0x30 = Wake up #3
  delay(10); //must wait 160us, busy flag not available
  commandLCD(0x38); //Function set: 8-bit/2-line
  commandLCD(0x10); //Set cursor
  commandLCD(0x0c); //Display ON; Cursor ON
  commandLCD(0x06); //Entry mode set

  //Check if LCD is functioning
  commandLCD(0xC0); //Set cursor to bottom right corner - 0xC0 will return same address in 4 and 8-bit.
  byte response = checkBusy();
  if(response == 0xC0){ //Confirm that expected reponse was received
    commandLCD(0x02); //Home the cursor
    return true;
  }
  digitalWriteFast(_LCD_toggle_pin, LOW); //Turn off LCD power if LCD fails to respond
  digitalWriteFast(_LED_PWM_pin, LOW); //Turn off backlight if LCD fails to respond
  analogWrite(_contrast_pin, 0); //Turn off LCD contrast if LCD fails to respond
  DAC0_C0 = ~DAC_C0_DACEN; //Disable DAC pin DAC0 to save power on hibernate - https://github.com/duff2013/Snooze/issues/12
  return false;
}

//Set pins to output
void LCD::outputPins(){
  for(int a=0; a<_DB_length; a++){
    pinMode(_DB_pin_array[a], OUTPUT);
  }
  pinMode(_RS_pin, OUTPUT);
  pinMode(_RW_pin, OUTPUT);
  pinMode(_E_pin, OUTPUT);
  pinMode(_LCD_toggle_pin, OUTPUT);
  pinMode(_LED_PWM_pin, OUTPUT);
}

//Adjust LCD contrast
void LCD::setLCDcontrast(float contrast){
  int bit_contrast = round(contrast*4095);
  analogWrite(_contrast_pin, bit_contrast);
}

//PRIVATE------------------------------------------------------------------------------------------
//Toggle E pin to latch DB 
void LCD::latch(){
  digitalWriteFast(_E_pin, HIGH);
  delayMicroseconds(2);
  digitalWriteFast(_E_pin, LOW);
}

//Send a byte of data to the LCD
void LCD::sendDBchar(char i){
  //If in 4 pin mode, post 4 MSB then 4 LSB
  if (_DB_length == 4){
    for(int a=_DB_length-1; a>=0; a--){
      digitalWriteFast(_DB_pin_array[a], i & B10000000);
      i = i<<1;
    }
    latch();
    for(int a=_DB_length-1; a>=0; a--){
      digitalWriteFast(_DB_pin_array[a], i & B10000000);
      i = i<<1;
    }
    latch();      
  }
  //If in 8 pin mode, post all 8 bits
  else if (_DB_length == 8){
    for(int a=0; a<_DB_length; a++){
      digitalWriteFast(_DB_pin_array[a], i%2);
      i = i>>1;
    }
    latch();
  }
}

//Send a command
void LCD::commandLCD(char i){
  digitalWriteFast(_RS_pin, LOW); //Send instruction
  digitalWriteFast(_RW_pin, LOW); //Write
  sendDBchar(i);
}

//Send a character
void LCD::writeLCD(char i){
  digitalWriteFast(_RS_pin, HIGH); //Send data
  digitalWriteFast(_RW_pin, LOW); //Write
  sendDBchar(i);
}

//Check if LCD is busy
byte LCD::checkBusy(){
  //Set DB pins as input
  for(int a=_DB_length-1; a>=0; a--){
      pinMode(_DB_pin_array[a], INPUT_PULLUP);
  }
  //Send command to get busy flag
  digitalWriteFast(_RS_pin, LOW); //Send instruction
  digitalWriteFast(_RW_pin, HIGH); //Read
  
  //Get flag (DB7) and address counter (DB0-DB6)
  byte response = 0;
  for(int a=_DB_length-1; a>=0; a--){
    if(digitalRead(_DB_pin_array[a])){
      if (_DB_length == 4) bitSet(response, a+4); //If 4bit mode used, shift 4 bits to MSB position
      else bitSet(response, a);  
    }
  }

  outputPins(); //Set the pins back to output
  
  return response;
}
