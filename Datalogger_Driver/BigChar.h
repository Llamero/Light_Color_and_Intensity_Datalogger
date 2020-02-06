//************************************************************************
//*  A set of custom made large numbers for a 16x2 LCD using the
//* LiquidCrystal librabry. Works with displays compatible with the
//* Hitachi HD44780 driver.
//*
//* orginal developed  by Michael Pilcher  2/9/2010
//* there are 8 entries, 8 bytes per entry
//* these are the building blocks to make the numbers
//*
//* http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1265696343
//************************************************************************

#ifndef BigChar_h
#define BigChar_h

#include "Arduino.h"
#include "LiquidCrystalFast.h"

class BigChar
{
  public:
    BigChar(const LiquidCrystalFast& _gLCD);
    static int  DrawBigChar(int xLocation, int yLocation, char theChar);
  private:
    static void BigNumber_SendCustomChars(void);
    const static uint8_t gBigFontAsciiTableWide[];
    const static uint8_t gBigFontAsciiTable[];
    const static uint8_t gBigFontShapeTable[];
    LiquidCrystalFast _gLCD;
};

#endif
