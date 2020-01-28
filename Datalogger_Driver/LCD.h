/*
  NHD-0420H1Z-FL-GBW-33V3.h - Library for the Newhaven Display NHD-0420H1Z-FL-GBW-33V3
*/
#ifndef Morse_h
#define Morse_h

#include "Arduino.h"

class LCD
{
  public:
    LCD(int *DB_pin_array, int DB_length, int RS_pin, int RW_pin, int E_pin, int LCD_toggle_pin, int LED_PWM_pin, int contrast_pin, int resolution);
    boolean initializeLCD();
    void outputPins();
    void setLCDcontrast(float contrast);
    void setLCDbacklight(float intensity);
    void disableDisplay();
    void displayCharArray(char t[][20], int i);

  private:
    int *_DB_pin_array; //Array or 4 or 8 DB pins for driver
    int _RS_pin; //RS pin
    int _RW_pin; //RW pin
    int _E_pin; //E pin
    int _DB_length; //Number of DB pins based on array length (4 or 8)
    int _LCD_toggle_pin; //Set to high to power on LCD
    int _LED_PWM_pin; //Drive LED backlight intensity
    int _contrast_pin; //DAC pin for addjusting diplay contrast
    int _max_analog; //Analog write resolution

    void latch(); //Toggle E pin to latch DB
    void sendDBchar(char i); //Send a byte of data to the LCD
    void commandLCD(char i); //Send a command
    void writeLCD(char i); //Send a character
    byte checkBusy(); //Returns the busy flag and address counter of LCD
    boolean monitorPresent(); //Check whether monitor is attached to PCB
};

#endif
