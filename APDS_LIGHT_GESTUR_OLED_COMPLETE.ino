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

int switch1;                      //스위치 값을 저장할 변수
int Key_off_flag = 0;             //스위치 채터링을 방지할 플래그
int adps_mode = 0;                //제스쳐 센서 모드와 조도 센서 모드를 판별할 변수
char buf1[32];                    //OLED에 표시할 내용을 저장할 문자 배열
char buf2[32];
#include <Wire.h>
#include "Adafruit_APDS9960.h"
#include "OLED.h"
Adafruit_APDS9960 apds;
OLED display(4,5);

void setup() {
  display.begin();
  pinMode(0, INPUT_PULLUP);       //G0를 입력으로
  pinMode(2, OUTPUT);             //2번 LED를 출력으로

  apds.begin();                   //APDS9960 사용
}

void loop() {
  switch1 = digitalRead(0);       //스위치 값을 읽어 변수에 저장
  if(switch1 == 0)                //채터링 방지
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
  if (adps_mode >= 2) adps_mode = 0;    //모드 0 -> 조도센서, 모드 1 -> 제스쳐 센서
  
  if (adps_mode == 0) {           //조도센서 모드
    apds.enableGesture(false);    //조도센서를 활성화 할 때에 다른 센서와 데이터 충돌이 일어나기 때문에
    delay(10);                    //조도센서의 경우엔 다른 센서를 off한 상태에서 작동시켜야 함.
    apds.enableProximity(false);
    delay(10);
    apds.enableColor(true);
    delay(10);
    uint16_t r, g, b, c;
    float illuminance;
    apds.getColorData(&r, &g, &b, &c);
    sprintf(buf1, "Light Sensor  ");
    display.print(buf1, 0, 0);    //OLED에 표시
    display.print("        ", 1, 0);
    illuminance = (float)apds.calculateLux(r, g, b);    //밝기를 측정
    if (illuminance > 35) digitalWrite(2, LOW);         //밝을때는 LED ON
    else digitalWrite(2, HIGH);                         //어두울때는 LED OFF
  }

  if (adps_mode == 1) {
    apds.enableGesture(true);     //제스쳐 모드 활성화
    delay(1);
    apds.enableProximity(true);   //제스쳐 모드는 Proximity(원근 센서)와 함께 동작함.
    delay(1);
    digitalWrite(2, HIGH);        //LED는 OFF
    uint8_t gesture = apds.readGesture();   //제스쳐 값을 읽어서 변수에 저장
    sprintf(buf1, "Gesture Sensor");  //OLED에 제스쳐 값을 표시
    display.print(buf1, 0, 0);
    if(gesture == APDS9960_DOWN) display.print("Dir : v", 1, 0);
    if(gesture == APDS9960_UP) display.print("Dir : ^", 1, 0);
    if(gesture == APDS9960_LEFT) display.print("Dir : <", 1, 0);
    if(gesture == APDS9960_RIGHT) display.print("Dir : >", 1, 0);
  }
}
