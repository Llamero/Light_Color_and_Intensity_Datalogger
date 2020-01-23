/*
  NHD-0420H1Z-FL-GBW-33V3.h - Library for the Newhaven Display NHD-0420H1Z-FL-GBW-33V3
*/
#ifndef Morse_h
#define Morse_h

#include "Arduino.h"

class LCD
{
  public:
    LCD(int *DB_pin_array, int DB_length, int RS_pin, int RW_pin, int E_pin);
    void initializeLCD();
    void outputPins();

  private:
    int *_DB_pin_array; //Array or 4 or 8 DB pins for driver
    int _RS_pin; //RS pin
    int _RW_pin; //RW pin
    int _E_pin; //E pin
    int _DB_length; //Number of DB pins based on array length (4 or 8)

    void latch(); //Toggle E pin to latch DB
    void sendDBchar(char i); //Send a byte of data to the LCD
    void commandLCD(char i); //Send a command
    void writeLCD(char i); //Send a character
};

#endif
