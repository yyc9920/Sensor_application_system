#include <Wire.h>
#include "OLED.h"
int encoder0PinA = 12;
int encoder0PinB = 13;
int encoder0Pin_RST = 2;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int n = LOW;
int Rst = LOW;
int ev_a[5];
char ev[5];
OLED display(4,5);
char strdata[100];
 
void setup() {
  display.begin();
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(encoder0Pin_RST, INPUT);
Serial.begin(9600);
}
void loop() {
 
  n = digitalRead(encoder0PinA);
       if((encoder0PinALast == LOW) && (n == HIGH)) {
        if(digitalRead(encoder0PinB) == LOW)
        { encoder0Pos--; } else { encoder0Pos++; }
        if(encoder0Pos>=0){
        ev_a[0] = encoder0Pos/100 + 48;
        ev_a[1] = (encoder0Pos%100)/10 + 48;
        ev_a[2] = encoder0Pos%10 + 48;
        ev[0] = ev_a[0];
        ev[1] = ev_a[1];
        ev[2] = ev_a[2];
        display.print(" ", 1, 0);
        display.print(ev, 1, 1);
        } else {
          ev_a[0] = -encoder0Pos/100 + 48;
          ev_a[1] = (-encoder0Pos%100)/10 + 48;
          ev_a[2] = -encoder0Pos%10 + 48;
          ev[0] = ev_a[0];
          ev[1] = ev_a[1];
          ev[2] = ev_a[2];
          display.print("-", 1, 0);
          display.print(ev, 1, 1);
        }
         } encoder0PinALast = n;
           Rst = digitalRead(encoder0Pin_RST);
       if(Rst == 0) {
        encoder0Pos  = 0;
        display.print(ev, 1, 0);
        }
}
