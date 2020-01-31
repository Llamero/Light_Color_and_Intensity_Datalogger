/*
  NHD-0420H1Z-FL-GBW-33V3.cpp - Library for the Newhaven Display NHD-0420H1Z-FL-GBW-33V3
*/

#include "Arduino.h"
#include "LCD.h"

LCD::LCD(int *DB_pin_array, int DB_length, int RS_pin, int RW_pin, int E_pin, int LCD_toggle_pin, int LED_PWM_pin, int contrast_pin, int resolution){
  _DB_pin_array = DB_pin_array;
  _RS_pin = RS_pin;
  _RW_pin = RW_pin;
  _E_pin = E_pin;
  _DB_length = DB_length;
  _LCD_toggle_pin = LCD_toggle_pin;
  _LED_PWM_pin = LED_PWM_pin;
  _contrast_pin = contrast_pin;
  _max_analog = (1<<resolution)-1; //Calcualte maximum value for analog write
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
  latch(); //command 0x30 = Wake up #2
  delay(10); //must wait 160us, busy flag not available
  latch(); //command 0x30 = Wake up #3
  delay(10); //must wait 160us, busy flag not available
  if(_DB_length == 8){
    commandLCD(0x38); //Function set: 8-bit/2-line
    commandLCD(0x10); //Set cursor
    commandLCD(0x0c); //Display ON; Cursor ON
    commandLCD(0x06); //Entry mode set
    commandLCD(0x01); //Clear display
  }
  else if(_DB_length == 4){
    commandLCD(0x20); //Function set: 4-bit interface
    commandLCD(0x28); //Function set: 4-bit/2-line
    commandLCD(0x10); //Set cursor
    commandLCD(0x0F); //Display ON; Blinking cursor
    commandLCD(0x06); //Entry Mode set
  }
  
  //Check that monitor is attached and responding
  if(monitorPresent()){
    return true;
  }
  else{
    disableDisplay();
    return false;
  }
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
  //Force 0-1 range
  if(contrast > 1.0){
    contrast = 1.0;
  }
  else if(contrast < 0){
    contrast = 0;
  }
  int bit_contrast = round((1-contrast) * _max_analog)>>2; //Values will automatically get mapped to max of 12-bit - https://www.pjrc.com/teensy/td_pulse.html; contrast is also inverted and maxes out at about 1/4 full range
  
  analogWrite(_contrast_pin, bit_contrast);
  Serial.println(bit_contrast);
}

//Adjust backlightintensity
void LCD::setLCDbacklight(float intensity){
  //Force 0-1 range
  if(intensity > 1.0){
    intensity = 1.0;
  }
  else if(intensity < 0){
    intensity = 0;
  }
  if(intensity > 0){
    int bit_intensity = round(intensity * _max_analog);
    analogWrite(_LED_PWM_pin, bit_intensity);
  }
  else{ //Fully shutdown 
    digitalWrite(_LED_PWM_pin, LOW);
  }
}

//Fully shutdown display into lowest power state
void LCD::disableDisplay(){
  digitalWriteFast(_LCD_toggle_pin, LOW); //Turn off LCD power
  digitalWriteFast(_LED_PWM_pin, LOW); //Turn off backlight
  analogWrite(_contrast_pin, 0); //Turn off LCD contrast
  DAC0_C0 = (unsigned char) ~DAC_C0_DACEN; //Disable DAC pin DAC0 to save power on hibernate - https://github.com/duff2013/Snooze/issues/12 - unsigned char to fix warning - https://www.avrfreaks.net/forum/warning-large-integer-implicitly-truncated-unsigned-type
}

void LCD::displayCharArray(char t[][20], int line1, int line2, int line3, int line4){
  commandLCD(0x01); //Clear display
  int row;
  for(int a=0; a<80; a++){
    if(a<20) row = line1;
    else if(a<40) row = line3;
    else if(a<60) row = line2;
    else row = line4;
    int col = a%20;
    if(row >= 0) writeLCD(t[row][col]);
    else writeLCD(' ');
  }
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
    if(i == 0x30 || i == 0x20) return; //For these two commands - only send the four MSB
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
  byte r = 255;
  byte count = 255;
  if(i != 0x30 && i != 0x20){ //If command is not initializing command
    while(r >= 128 && _DB_length>4 && count--){ //Wait until busy flag is cleared
      r = checkBusy();
    }
  }
}

//Send a character
void LCD::writeLCD(char i){
  digitalWriteFast(_RS_pin, HIGH); //Send data
  digitalWriteFast(_RW_pin, LOW); //Write
  sendDBchar(i);
  byte r = 255;
  byte count = 255;
  while(r >= 128 && _DB_length>4 && count--){ //Wait until busy flag is cleared
    r = checkBusy();
  }
}

//Check if LCD is busy
byte LCD::checkBusy(){
    //Set DB pins as input
  for(int a=_DB_length-1; a>=0; a--){
      pinMode(_DB_pin_array[a], INPUT_PULLUP);
  }
  delayMicroseconds(100); //must wait at least 80us before checking BF - https://www.newhavendisplay.com/app_notes/ST7066U.pdf
  
  //Send command to get busy flag
  digitalWriteFast(_RS_pin, LOW); //Send instruction
  digitalWriteFast(_RW_pin, HIGH); //Read
  digitalWriteFast(_E_pin, HIGH); //Enable BF
  delayMicroseconds(2000);
  //Get flag (DB7) and address counter (DB0-DB6)
  byte response = 0;
  for(int a=_DB_length-1; a>=0; a--){
    if(digitalRead(_DB_pin_array[a])){
      if (_DB_length == 4) bitSet(response, a+4); //If 4bit mode used, shift 4 bits to MSB position
      else bitSet(response, a);  
    }
  }
  digitalWriteFast(_E_pin, LOW); //Reset BF
  //If in 4-bit mode - get 4 LSB next
  if (_DB_length == 4){
    digitalWriteFast(_E_pin, HIGH); //Enable BF
    delayMicroseconds(1000);
    digitalWriteFast(_E_pin, LOW); //Reset BF
  }
  outputPins(); //Set the pins back to output
  return response;
}

//Check that LCD is connected and functioning correctly
boolean LCD::monitorPresent(){
  int a;
  //Check all DDRAM addresses and confrim checkBusy returns the same address
  for(a = 0; a<=127; a++){
    commandLCD(128+a);
    byte r = checkBusy();
    if(r != a && a != 0x28 && a != 0x68) return false;
    else if((a == 0x28 && r != 0x40) || (a == 0x68 && r != 0x0)) return false; //These two slots roll over to the next line
  }
  return true;
}
