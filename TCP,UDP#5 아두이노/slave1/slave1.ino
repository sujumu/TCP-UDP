/*
 * 예제125-1-2 (슬레이브1코드)
 * 마스터가 슬레이브1과 슬레이브2에게 아래와 같이 전송한다
 * (Request) 0x0F6(id) 0x00(job) = ID가 0x00인 슬레이브의 통신상태 점검
 * (Response) 0x0F6(id) 0x00(job) (byte)'0' (byte)'K' = OK를 회신할 수 있도록 하자
 */

#include <SPI.h>
#include <mcp2515.h>

#define slave1 0x0F6
#define slave2 0x736

#define led 2

struct can_frame canMsg;
struct can_frame sendMsg;
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    //자~~ 데이터 들어온다
    //내건지 확인을 해봐야겠다~
    if(canMsg.can_id != slave2) return; 
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
      Serial.print(canMsg.data[i],HEX);
      Serial.print(" ");
    }

    Serial.println();    
      
    if(canMsg.data[0] == 0x01){
      digitalWrite(led,!digitalRead(led));
    }
      
    //응답하는 부분
    sendMsg.can_id  = canMsg.can_id;
    sendMsg.can_dlc = canMsg.can_dlc;
    sendMsg.data[0] = digitalRead(led);
    sendMsg.data[1] = 0x00;
    sendMsg.data[2] = 0x00;
    sendMsg.data[3] = 0x00;
    sendMsg.data[4] = 0x00;
    sendMsg.data[5] = 0x00;
    sendMsg.data[6] = 0x00;
    sendMsg.data[7] = 0x00;
    mcp2515.sendMessage(&sendMsg);
  }
}