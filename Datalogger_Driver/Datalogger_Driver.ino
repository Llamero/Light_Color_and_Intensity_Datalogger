#include "LCD.h"
int arr[] = {0,9,6,7};
int b = sizeof(arr)/sizeof(arr[0]);
LCD lcd(arr, b, 10, 6, 8);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  wakeUSB();
  delay(1);
  Serial.println(b);
  lcd.initializeLCD();
}

void loop() {
  // put your main code here, to run repeatedly:

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
