#include <SPI.h>
#include <mcp2515.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

#define slave1 0x0F6
#define slave2 0x036

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

String device_id = "#device2";

struct can_frame canMsg2;
MCP2515 mcp2515(D10); //CS핀이 몇번핀인가?


void setup() {
  Serial.begin(115200);
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

  //CAN통신 모듈 설정하는 부분
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("Example: Write to CAN");
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

    /*
    //실제로 원하는 메시지를 전송하는 부분
    if(millis() - dt2 > 2000){
      dt2 = millis();
      String output;
        StaticJsonDocument<200> doc;

        doc["from"] = device_id;
        doc["to"] = "#app1";
        doc["msg"] = "나는 보드 1번입니다!";
        
        serializeJson(doc, output);
        client.println(output);
    }
    */
    
    //서버에서 전송한 메시지를 클라이언트도 받아야 한다!
    if(client.available()){
      //서버에서 전송한 메시지가 있다!
      Serial.println("메시지가 수신되었다");
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
        String slave_id = doc["msg"]["id"];
        String recv = doc["msg"]["req"];

        //16개의 char array를 2개씩 쪼개서 8 bytes array로 변환한다!
        if(recv.length() != 16) return; //예외

        byte output[8];
        for(int i = 0,j=0;i<16;i+=2,j++){
          int num1 = get_number(recv[i]);
          int num2 = get_number(recv[i+1]);
          output[j] = num1*16 + num2;
        }

        //슬레이브 id를 문자열에서 숫자로 바꾸어라!
        //0F6   (000~7FF)   7FF:2047
        unsigned int myid = get_number(slave_id[0])*16*16
        +get_number(slave_id[1])*16
        +get_number(slave_id[2]);
        
        
        /*
        Serial.print("["+from+"] ");
        Serial.print(myid,HEX);
        Serial.print(",");
        for(int i = 0;i<8;i++){
          Serial.print(output[i],HEX);
          Serial.print(",");
        }
        Serial.println();
        */

        //output : data
        //Request slave1 
        
        mcp2515_send(myid,output); //request
      
        //Response
        byte buff[8];
        unsigned int id = mcp2515_receive(buff);
        if(id == -1){
          Serial.println("슬레이브1 오프라인!");
          String output;
          StaticJsonDocument<200> doc;
  
          doc["from"] = device_id;
          doc["to"] = "#device1";
          
          JsonObject msg = doc.createNestedObject("msg");
          msg["id"] = slave_id;
          msg["result"] = "NO";
          msg["res"] = "";
          
          serializeJson(doc, output);
          client.println(output);
          Serial.println(output);
        }else{
          //데이터 정상수신
          //프린트
          /*
          Serial.print("수신ID=");
          Serial.println(id,HEX);
          for(int i = 0;i<8;i++){
            Serial.print((char)recv[i]);
          }
          Serial.println();
          */

          //반환할 8바이트 배열을 다시 문자열로 만들어야한다!
          String return_data="";
          for(int i = 0;i<8;i++){
            if(buff[i] < 10){
              return_data += '0' + String(buff[i],HEX);  
            }else{
              return_data += String(buff[i],HEX);  
            }
          }
          
          String output;
          StaticJsonDocument<200> doc;
  
          doc["from"] = device_id;
          doc["to"] = "#device1";
          
          JsonObject msg = doc.createNestedObject("msg");
          msg["id"] = slave_id;
          msg["result"] = "OK";
          msg["res"] = return_data;
          
          serializeJson(doc, output);
          client.println(output);
          Serial.println(output);
        }
       
      }
    }
  }
  client.stop();
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

void mcp2515_send(unsigned int id, byte data[]){
  struct can_frame canMsg1;
  canMsg1.can_id  = id; //슬레이브1의 ID
  canMsg1.can_dlc = 8;
  for(int i =0;i<8;i++){
    canMsg1.data[i] = data[i];
  }
  mcp2515.sendMessage(&canMsg1);
  Serial.println("[마스터에서 슬레이브로 보낸 메시지]");
  Serial.println(id,HEX);
  for (int i = 0; i<canMsg1.can_dlc; i++)  {  // print the data
      Serial.print(canMsg1.data[i],HEX);
      Serial.print(" ");
  }
  Serial.println();
}
unsigned int mcp2515_receive(byte recv[]){
  struct can_frame canMsg;
  unsigned long t = millis(); //이 타이밍의 시간을 측정
  while(1){
    if(millis() - t > 1000){
      //루프 진입후 3초가 지난 시점
      //Serial.println("슬레이브1 오프라인");
      return -1;
      break;
    }
    //나 슬레이브1의 응답데이터가 있을때까지 무조건 기다릴거야!
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      for(int i =0;i<8;i++){
        recv[i] = canMsg.data[i];
      }
      return canMsg.can_id;
      break;
    }
  }
}