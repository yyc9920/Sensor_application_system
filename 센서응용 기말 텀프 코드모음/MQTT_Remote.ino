#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "KT_GiGA_2G_Wave2_87EA"; // 와이파이 이름
const char* password = "bh63cb0545"; // 와이파이 비밀번호
#define mqtt_server "tailor.cloudmqtt.com" // MQTT server 주소
#define mqtt_port 16417 // port 번호
#define cds_topic "cds" // topic (조도 센서)
#define ultsonic_topic "ultsonic" // topic (초음파 센서)
#define enc_topic "encoder" // topic (엔코더)
#define mqtt_user "nsiilwwe" // username
#define mqtt_password "tAboZHax9-Ue" // password
#define trigPin 13                   // trigPin을 13으로 설정합니다.
#define echoPin 12                // echoPin을 12로 설정합니다.
int pinLED = 2;                   // LED pin => 2
int cds;                          // 조도센서 값을 저장하는 변수
int loop_cnt=0;                   // Publish 주기를 정해주는 카운터 변수
unsigned long enc_value;          // Subscribe한 엔코더 캐릭터배열을 정수형으로 바꾼 값을 저장
char buf[5];                      // 조도 센서와 초음파 센서를 Publish하기 위해
char buf1[5];                     // 캐릭터배열 선언 후 센서값(정수)를 변환하여 저장
char buf_enc[5];                  // Subscribe한 엔코더 값을 정수형으로 바꿔주기 위해 캐릭터배열 선언
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(9600);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) delay(500);
pinMode(pinLED, OUTPUT);
pinMode(trigPin, OUTPUT);   // trigPin 핀을 출력핀으로 설정합니다.
pinMode(echoPin, INPUT);    // echoPin 핀을 입력핀으로 설정합니다.
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
}
void loop() {
   long duration, distance;                   // 각 변수를 선언합니다.
  digitalWrite(trigPin, LOW);                 // trigPin에 LOW를 출력하고
  delayMicroseconds(2);                    // 2 마이크로초가 지나면
  digitalWrite(trigPin, HIGH);                // trigPin에 HIGH를 출력합니다.
  delayMicroseconds(10);                  // trigPin을 10마이크로초 동안 기다렸다가
  digitalWrite(trigPin, LOW);                // trigPin에 LOW를 출력합니다.
  duration = pulseIn(echoPin, HIGH);   // echoPin핀에서 펄스값을 받아옵니다.
  distance = duration * 17 / 1000;          //  duration을 연산하여 센싱한 거리값을 distance에 저장합니다.
cds = analogRead(A0);                 // 조도센서 값을 저장합니다.
sprintf(buf, "%d", cds);              // 정수형 센서값을 캐릭터배열로 변환
sprintf(buf1, "%d", distance);        // 정수형 센서값을 캐릭터배열로 변환
loop_cnt++;                           // Publish 주기를 정해주는 카운터 변수
if (!client.connected()) {
client.connect("ESP8266Client1", mqtt_user, mqtt_password);
client.subscribe(enc_topic);          // 엔코더 토픽을 Subscribe
}
delay(10);
if(loop_cnt >= 80){
  client.publish(cds_topic, buf);
  client.publish(ultsonic_topic, buf1);   // 조도센서와 초음파센서를 Publish
  loop_cnt = 0;                           // 카운터 변수 초기화
}

client.loop();
}
void callback(char* topic, byte* payload, unsigned int length) {
String Msg = "";
int i=0;
while (i<length) Msg += (char)payload[i++];
Msg.toCharArray(buf_enc, 5);    // 메세지 스트링, 엔코더값을 캐릭터배열로 변환
Serial.print("encoder : ");
Serial.println(buf_enc);        // 시리얼로 확인할 수 있도록 시리얼 프린트
enc_value = atoi(buf_enc);      // 엔코더 캐릭터변수를 정수값으로 변환.
 if (enc_value%2) {             // MQTT 브로커에 들어오는 encoder값이
  digitalWrite(2, LOW);         // 홀수일 때 LED ON
 } else digitalWrite(2, HIGH);  // 짝수일 때 LED OFF
}
