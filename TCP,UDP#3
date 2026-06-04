#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <Servo.h>
Servo myservo;

#define led D3

//와이파이 공유기의 id비밀번호가 들어가야 인터넷에 접속이 가능하겠죠?
#ifndef STASSID
#define STASSID "U+Net61DF"
#define STAPSK  "0108016660"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

//클라이언트가 서버에 연결을 해야하는데.. 서버의 주소가 필요하다!
const char* host = "192.168.219.102";
//서버의 ip주소는 전체적인 주소의 일부분 상세주소인 포트번호가 필요하다!
const uint16_t port = 60000;

ESP8266WiFiMulti WiFiMulti;

int degree = 0;

//셋업함수는 아두이노가 부팅되면 딱 한번 실행되는부분!
//와이파이 공유기와 연결하는 작업을 진행한다!
void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);//접속확인용 LED
  myservo.attach(D4);
  myservo.write(0);
  
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {
  //서버주소가 ~~인데 ~~에 연결할거다!
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  //클라이언트가 서버에 접속을 시도한다!(5초마다 한다!)
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    digitalWrite(led,LOW); //작동확인등 OFF
    delay(5000);
    return;
  }
  digitalWrite(led,HIGH); //작동확인등 ON
  //서버와 접속이 성공적으로 진행이된 경우!

  unsigned long dt = millis();
  //서버와 클라이언트가 연결이 보장이된 경우에 while루프가 작동하고
  //연결이 끊어지면 while루프가 멈춘다!
  while(client.connected()){
    //여기안은 클라이언트와 서버가 접속중을때 활성화된다!
    //1초마다 서버에게 메시지를 전송한다!
    if(millis() - dt > 100){
      dt = millis();
      client.println("연결!");

      //JSON
      String output;
      StaticJsonDocument<200> doc;
      //{"device":"servo","d_id":1,"value":90}
      doc["device"] = "servo";
      doc["d_id"] = 1; 
      doc["value"] = degree; 
      serializeJson(doc, output);
      //Serial.println(output);
      
      client.println(output);
    }
    
    //서버에서 전송한 메시지를 클라이언트도 받아야 한다!
    if(client.available()){
      //서버에서 전송한 메시지가 있다!
      Serial.println("메시지가 수신되었다");
      String msg = client.readStringUntil('\n');

      //msg안에 JSON이 타고있어요!
      //{"device_id":3,"msg":"나는 C# TCP 서버이다!!"}

      StaticJsonDocument<200> doc;

      deserializeJson(doc, msg);

      //{"device":"servo","d_id":1,"value":90}
      String device = doc["device"];
      int d_id = doc["d_id"]; 
      int value = doc["value"]; 

      if(device == "servo"){
        if(d_id == 1){
          degree = value;
           myservo.write(value);
        }
      }
    }
  }
  client.stop();

}
