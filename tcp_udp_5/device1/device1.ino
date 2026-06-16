#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

//와이파이 공유기의 id비밀번호가 들어가야 인터넷에 접속이 가능하겠죠?
#ifndef STASSID
#define STASSID "U+Net61DF"
#define STAPSK  "0108016660"
#endif

#define button1 D3
#define button2 D4

const char* ssid     = STASSID;
const char* password = STAPSK;

//클라이언트가 서버에 연결을 해야하는데.. 서버의 주소가 필요하다!
const char* host = "192.168.219.102";
//서버의 ip주소는 전체적인 주소의 일부분 상세주소인 포트번호가 필요하다!
const uint16_t port = 60000;

ESP8266WiFiMulti WiFiMulti;

String device_id = "#device1";

bool slave_sw = false;

// Use WiFiClient class to create TCP connections
WiFiClient client;

//셋업함수는 아두이노가 부팅되면 딱 한번 실행되는부분!
//와이파이 공유기와 연결하는 작업을 진행한다!
void setup() {
  Serial.begin(115200);
  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);
  
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

  

  //클라이언트가 서버에 접속을 시도한다!(5초마다 한다!)
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
  //서버와 접속이 성공적으로 진행이된 경우!

  bool is_accepted = false;

  unsigned long dt = millis();//서버하고 소통하는 스레드
  unsigned long dt2 = millis(); //monitor1
  //서버와 클라이언트가 연결이 보장이된 경우에 while루프가 작동하고
  //연결이 끊어지면 while루프가 멈춘다!
  while(client.connected()){
    //여기안은 클라이언트와 서버가 접속중을때 활성화된다!
    //1초마다 서버에게 메시지를 전송한다!
    if(millis() - dt > 1000){
      dt = millis();
      if(!is_accepted){
        //서버하고 인사를 안한상태
        client.println(device_id);
      }else{
        //서버에게 나 접속하고 있어요~ 라는 데이터를 보낸다
        String output;
        StaticJsonDocument<200> doc;

        doc["from"] = device_id;
        doc["to"] = "#server";
        doc["msg"] = "OK";
        
        serializeJson(doc, output);
        client.println(output);
        
      }
    }

    if(digitalRead(button1) == LOW){
      can_request(1);
      delay(300);
    }else if(digitalRead(button2) == LOW){
      can_request(2);
      delay(300);
    }

    /*
    //실제로 원하는 메시지를 전송하는 부분
    if(millis() - dt2 > 2000){
      dt2 = millis();
      
    }
    */
    
    //서버에서 전송한 메시지를 클라이언트도 받아야 한다!
    if(client.available()){
      //서버에서 전송한 메시지가 있다!
      //Serial.println("메시지가 수신되었다");
      String msg = client.readStringUntil('\n');
      //서버에서 OK라는 응답을 하게 된다면~
      if(msg == "#OK"){
        Serial.println("서버하고 명함 주고 받는 절차를 마무리했다!");
        is_accepted = true;
      }else{
        //msg : JSON에서 object로 바꾸어줘야한다!
      
        StaticJsonDocument<150> doc;

        deserializeJson(doc, msg);
      
        String from = doc["from"]; 
        String result = doc["msg"]["result"];
        String res = doc["msg"]["res"];
        String id = doc["msg"]["id"];

        if(result == "OK"){
          //데이터 = res
          byte output[8];
          for(int i = 0,j=0;i<16;i+=2,j++){
            int num1 = get_number(res[i]);
            int num2 = get_number(res[i+1]);
            output[j] = num1*16 + num2;
          }
          Serial.print("["+from+"] "+id + ", " + result + ", ");
          for(int i = 0;i<8;i++){
            Serial.print(output[i]);
            Serial.print(",");
          }
          Serial.println();
        }else if(result == "NO"){
          Serial.println("["+from+"] "+id + ", " + result);
        }
      }
    }
  }
  client.stop();

}

void can_request(int slave_num){

  String output;
  StaticJsonDocument<200> doc;

  doc["from"] = device_id;
  doc["to"] = "#device2";
        
  JsonObject msg = doc.createNestedObject("msg");
  if(slave_num == 1){
    msg["id"] = "0F6"; //736 0F6    
  }else if(slave_num == 2){
    msg["id"] = "736"; //736 0F6  
  }

  msg["req"] = "0100000000000000";
  
  serializeJson(doc, output);
  client.println(output);
}

int get_number(char c){
  int output=0;
  if(c >= '0' && c <= '9'){
    output = c - '0';
  }
  if(c >= 'a' && c <= 'f'){
    output = c - 'a' + 10;
  }
  if(c >= 'A' && c <= 'F'){
    output = c - 'A' + 10;
  }

  return output;
}
