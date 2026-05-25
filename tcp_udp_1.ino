#include <ESP8266WiFi.h>

//사물인터넷보드가 무선공유기에 연결되어야한다!
//무선공유기 ID PW
#ifndef STASSID
#define STASSID "popcorn"
#define STAPSK  "11213144"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

//IoT보드가 서버역할을 하겠다!(1~65536)
WiFiServer server(60000); //서버를 60000번 포트에 열겠다!

//셋업함수 -> 무선공유기하고 연결하는 작업!
void setup() {
  Serial.begin(115200); //PC-IoT보드간 통신(결과를 PC에 출력하기 위한)

  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  //서버가 클라이언트의 접속을 기다리는것을 listen한다라고 표현한다!
  WiFiClient client = server.available();
  //클라이언트가 접속한 상태가 아니라면 = 소켓이 생성된 상태가 아니라면~
  if (!client) {
    return;
  }
  Serial.println("새로운 클라이언트의 등장!");

  unsigned long t = millis();
  //서버에 클라이언트가 접속중일때 while루프가 작동된다!
  while(client.connected()){
    //클라이언트쪽에서 서버쪽으로 뭔가 전송한 값이 있다고 가정한다!
    if(client.available()){
      //클라이언트가 뭔가 전송한 값이 존재한다!
      //받는다!
      //(약속) 상호간의 송수신하는 데이터의 끝에는 개행문자(\n)을 붙혀서 전송할것!
      //클라이언트가 전송한 메시지를 처음부터 끝까지 읽어들이겠다!
      String data = client.readStringUntil('\n');// readline()
      Serial.println(data);
    }
    if(millis() - t > 1000){
      t = millis();
      //1초마다 한번씩 실행되는 부분
      client.print("서버에서 클라이언트로 보내는 메시지!");
    }
  }

  Serial.println("클라이언트와 접속이 끊어졌다!");
}