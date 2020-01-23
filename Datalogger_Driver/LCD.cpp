/*
  NHD-0420H1Z-FL-GBW-33V3.cpp - Library for the Newhaven Display NHD-0420H1Z-FL-GBW-33V3
*/

#include "Arduino.h"
#include "LCD.h"

LCD::LCD(int *DB_pin_array, int DB_length, int RS_pin, int RW_pin, int E_pin){
  _DB_pin_array = DB_pin_array;
  _RS_pin = RS_pin;
  _RW_pin = RW_pin;
  _E_pin = E_pin;
  _DB_length = DB_length;
}

//Start display from power on - http://www.newhavendisplay.com/specs/NHD-0420H1Z-FL-GBW-33V3.pdf
void LCD::initializeLCD(){
  outputPins();
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
  for(int a=0; a<_DB_length; a++){
    Serial.print(_DB_pin_array[a]);
    Serial.print(" ");
  }
  Serial.println();
}

//Set pins to output
void LCD::outputPins(){
  for(int a=0; a<_DB_length; a++){
    pinMode(_DB_pin_array[a], OUTPUT);
  }
  pinMode(_RS_pin, OUTPUT);
  pinMode(_RW_pin, OUTPUT);
  pinMode(_E_pin, OUTPUT);
}

//------------------------------------------------------------------------------------------
//Toggle E pin to latch DB 
void LCD::latch(){
  digitalWriteFast(_E_pin, HIGH);
  delayMicroseconds(2);
  digitalWriteFast(_E_pin, LOW);
}

//Send a byte of data to the LCD
void LCD::sendDBchar(char i){
  //If in 4 pin mode, post 4 LSB then 4 MSB
  if (_DB_length == 4){
    for(int a=0; a<_DB_length; a++){
      digitalWriteFast(_DB_pin_array[a], i%2);
      i = i>>1;
    }
    latch();
    for(int a=0; a<_DB_length; a++){
      digitalWriteFast(_DB_pin_array[a], i%2);
      i = i>>1;
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
