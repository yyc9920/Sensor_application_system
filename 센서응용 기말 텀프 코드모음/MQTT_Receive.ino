#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "OLED.h"
OLED display(4,5);
const char* ssid = "KT_GiGA_2G_Wave2_87EA"; // 와이파이 이름
const char* password = "bh63cb0545"; // 와이파이 비밀번호
#define mqtt_server "tailor.cloudmqtt.com" // MQTT server 주소
#define mqtt_port 16417 // port 번호
#define cds_topic "cds" // topic (조도 센서)
#define ultsonic_topic "ultsonic" // topic (초음파 센서)
#define enc_topic "encoder" // topic (엔코더)
#define mqtt_user "nsiilwwe" // username
#define mqtt_password "tAboZHax9-Ue" // password
#define LED_R   12          // RGB LED에서 RED 핀 => 12
#define LED_G   13          // RGB LED에서 GREEN 핀 => 13
#define LED_B   14          // RGB LED에서 BLUE 핀 => 14
int pinLED = 2;             // LED pin => 2
int pinSwitch = 0;          // Switch pin => 0
int pinState = HIGH;
int encoder0PinA = 12;      // 엔코더 핀설정, 변수 설정
int encoder0PinB = 13;
int encoder0Pin_RST = 2;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int n = LOW;
int Rst = LOW;
char buf_cds[5];          // 각종 센서 값을 저장할 캐릭터배열 변수 선언
char buf_ultsonic[5];
char buf[5];
char enc_value[5];
char buf_lcd1[30];        // OLED에 센서값을 표시할 캐릭터배열 변수 선언
char buf_lcd2[30];
unsigned int cnt=0;       // 스위치가 눌린 시간을 저장하는 카운터변수
int ult_cnt=1;            // Subscribe한 초음파 센서 값과 지속시간에 따라 RGB LED를 다르게 해주기 위한 카운터변수
int loop_cnt = 0;
int ult_cnt_tmp, ult_cnt_tmp1, ult_flag;   // Subscribe한 초음파 센서 값과 지속시간에 따라 RGB LED를 다르게 해주기 위한 변수
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(9600);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) delay(500);
pinMode(pinLED, OUTPUT);
pinMode(pinSwitch, INPUT_PULLUP);
pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(encoder0Pin_RST, INPUT);
pinMode(LED_R, OUTPUT);
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);
pinState = digitalRead(pinSwitch);
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
display.begin();
}
void loop() {
sprintf(buf_lcd1, "CDS   ");
display.print(buf_lcd1, 0, 0); 
sprintf(buf_lcd2, "Ultrasonic");
display.print(buf_lcd2, 2, 0); 
// 엔코더 값을 처리하는 시퀀스
n = digitalRead(encoder0PinA);
       if((encoder0PinALast == LOW) && (n == HIGH)) {
        if(digitalRead(encoder0PinB) == LOW)
        { encoder0Pos--; } else { encoder0Pos++; }
        }
        encoder0PinALast = n;
           Rst = digitalRead(encoder0Pin_RST);
       if(Rst == 0) {
        encoder0Pos  = 0;
        }
        if (encoder0Pos<0){
          sprintf(enc_value, "%d", -encoder0Pos);
        }else sprintf(enc_value, "%d", encoder0Pos);    // 엔코더 정수형을 캐릭터배열로 변환
        
if (!client.connected()) {
client.connect("ESP8266Client", mqtt_user, mqtt_password);
client.subscribe(cds_topic);
client.subscribe(ultsonic_topic);
}
client.publish(enc_topic, enc_value);     // 엔코더 값을 Publish
delay(10);
loop_cnt++;
if (loop_cnt >= 3) {
  loop_cnt = 0;
  Serial.print("encoder : ");
  Serial.println(enc_value);              // 엔코더값을 시리얼에 
}
client.loop();
}
void callback(char* topic, byte* payload, unsigned int length) {
String Msg = "";
int i=0;
int ult_value, cds_value;
while (i<length) Msg += (char)payload[i++];

if(!strcmp(cds_topic, topic)){
  Msg.toCharArray(buf_cds, 5);      // 토픽이 cds일 경우 메세지를 캐릭터배열로 변환
//  Serial.print("cds : ");
//  Serial.println(buf_cds);
} else if (!strcmp(ultsonic_topic, topic)){
  Msg.toCharArray(buf_ultsonic, 5); // 토픽이 ultsonic일 경우 메세지를 캐릭터배열로 변환
  Serial.print("ultsonic : ");
  Serial.println(buf_ultsonic);
}

ult_value = atoi(buf_ultsonic);     // 초음파센서 캐릭터배열을 정수형으로 변환하여 ult_value에 저장.

if(ult_value <= 10) {                   // 애완동물이 집 안에 들어왔을 때
  if(ult_cnt<1000) ult_cnt_tmp1+=3;     // 집 안에 들어온 시간을 카운트
  ult_cnt = ult_cnt_tmp1;
  ult_cnt_tmp = 100;                    // 집 안에 있다가 밖에 나가면 카운터변수를 초기화
} else {
  if(ult_cnt>1) ult_cnt_tmp -= 3;       // 집 밖으로 나간 시간을 카운트
  ult_cnt = ult_cnt_tmp;
  ult_cnt_tmp1 = 100;                   // 집 밖에서 안으로 들어오면 카운터변수를 초기화
}

sprintf(buf, "%d", ult_cnt);
Serial.print("ult cnt : ");
Serial.println(buf);                    // 카운터변수를 시리얼에 출력 
if(ult_cnt >= 200) {                    // 집 안에 들어온 후 약 10초가 지나면 RGB LED -> G
  ult_flag = 2;
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}
else if (ult_cnt > 1) {                 // 집 안과 밖을 10초 내로 왔다갔다 할 때 RGB LED -> B
  ult_flag = 1;
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}
else if (ult_cnt == 1) {                // 집 밖으로 나간 후 약 10초가 지나면 RGB LED -> R
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

if (!digitalRead(pinSwitch)) {
  cnt++;
} else cnt = 0;
// 스위치를 누르면 OLED에 각 센서값을 출력
if (cnt >= 1)
{
  Serial.println(buf_cds);
  display.print("       ", 1, 0);  
  delay(1);
  display.print(buf_cds, 1, 0);
  Serial.println(buf_ultsonic);
  display.print("       ", 3, 0);  
  delay(1);
  display.print(buf_ultsonic, 3, 0);
}
}
