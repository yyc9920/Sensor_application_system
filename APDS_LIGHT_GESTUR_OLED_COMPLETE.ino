/***************************************************************************
  This is a library for the APDS9960 digital proximity, ambient light, RGB, and gesture sensor
  This sketch puts the sensor in color mode and reads the RGB and clear values.
  Designed specifically to work with the Adafruit APDS9960 breakout
  ----> http://www.adafruit.com/products/3595
  These sensors use I2C to communicate. The device's I2C address is 0x39
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

int switch1;
int Key_off_flag = 0;
int adps_mode = 0;
char buf1[32];
char buf2[32];
#include <Wire.h>
#include "Adafruit_APDS9960.h"
#include "OLED.h"
Adafruit_APDS9960 apds;
OLED display(4,5);

void setup() {
  display.begin();
  pinMode(0, INPUT_PULLUP);
  pinMode(2, OUTPUT);

  apds.begin();
}

void loop() {
  switch1 = digitalRead(0);
  if(switch1 == 0)
  {
    if(Key_off_flag == 1)
      {
        adps_mode++;
        Key_off_flag = 0;
      }
  }
  else {
    Key_off_flag = 1;
  }
  if (adps_mode >= 2) adps_mode = 0;
  
  if (adps_mode == 0) {
    apds.enableGesture(false);
    delay(10);
    apds.enableProximity(false);
    delay(10);
    apds.enableColor(true);
    delay(10);
    uint16_t r, g, b, c;
    float illuminance;
    //get the data and print the different channels
    apds.getColorData(&r, &g, &b, &c);
    sprintf(buf1, "Light Sensor  ");
    display.print(buf1, 0, 0);
    display.print("        ", 1, 0);
    illuminance = (float)apds.calculateLux(r, g, b);
    if (illuminance > 35) digitalWrite(2, LOW);
    else digitalWrite(2, HIGH);
  }

  if (adps_mode == 1) {
    apds.enableGesture(true);
    delay(1);
    apds.enableProximity(true);
    delay(1);
    digitalWrite(2, HIGH);
    uint8_t gesture = apds.readGesture();
    sprintf(buf1, "Gesture Sensor");
    display.print(buf1, 0, 0);
    if(gesture == APDS9960_DOWN) display.print("Dir : v", 1, 0);
    if(gesture == APDS9960_UP) display.print("Dir : ^", 1, 0);
    if(gesture == APDS9960_LEFT) display.print("Dir : <", 1, 0);
    if(gesture == APDS9960_RIGHT) display.print("Dir : >", 1, 0);
  }
}
