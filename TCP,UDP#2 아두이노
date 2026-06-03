#include <ESP8266WiFi.h> //ESP8266 보드를 와이파이에 연결하기 위한 라이브러리
#include <WiFiUdp.h> //UDP 통신을 사용하기 위한 라이브러리

//공유기에 접속하기 위한 와이파이 이름과 비밀번호를 설정
#ifndef STASSID
#define STASSID "U+Net61DF"      // 접속할 Wi-Fi 공유기 이름
#define STAPSK  "0108016660"     // 접속할 Wi-Fi 비밀번호
#endif

//ESP8266이 UDP 데이터를 수신할 포트 번호
unsigned int localPort = 60000;
//UDP로 수신할 데이터를 임시로 저장할 버퍼이다.
// UDP_TX_PACKET_MAX_SIZE는 라이브러리에서 정해둔 UDP 패킷 최대 크기이다.
// +1을 하는 이유는 문자열 끝을 표시하는 '\0' 문자를 추가하기 위해서이다.
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];

//ESP8266이 전송할 문자열
char ReplyBuffer[] = "반갑습니다.\r\n";
//UDP 통신을 담당하는 객체 생성
WiFiUDP Udp;
//1초마다 데이터를 보내기 위한 시간 저장 변수
unsigned long t =0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); //ESP8266을 스테이션 모드로 설정
  WiFi.begin(STASSID, STAPSK);

  //와이파이 연결이 완료될 때까지 반복해서 대기한다.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
}

void loop() {
  //수신된 UDP 패킷이 있는지 확인, 있으면 패킷의 크기 반환, 없으면 0반환
  int packetSize = Udp.parsePacket();
  // 수신된 UDP 패킷의 정보를 시리얼 모니터에 출력한다.
  // remoteIP()는 데이터를 보낸 장치의 IP 주소이다.
  // remotePort()는 데이터를 보낸 장치의 포트 번호이다.
  // destinationIP()는 패킷이 도착한 목적지 IP 주소이다.
  // localPort()는 ESP8266이 사용 중인 UDP 수신 포트이다.
  // ESP.getFreeHeap()은 현재 남아 있는 동적 메모리 크기를 의미한다.
  if (packetSize) {
    Serial.printf("Recived packet of size %d from %s:%d\n (to %s:%d, free heap = %d B)\n",
    packetSize,
    Udp.remoteIP().toString().c_str(),
    Udp.remotePort(),
    Udp.destinationIP().toString().c_str(),
    Udp.localPort(),
    ESP.getFreeHeap());
    
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //수신된 데이터의 끝에 \n을 추가한다
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);
  }
  if (millis()-t>1000) {
    t=millis();
    //해당 포트와 ip주소로 데이터를 보낼거다
    Udp.beginPacket("192.168.219.102",60001);
    //ReplBuffer에 저장된 문자열을 UDP 데이터로 전송한다.
    Udp.write(ReplyBuffer);

    //BeginPacke으로 시작한 패킷은 반드시 endPacket()으로 마무리시 실제 전송이 이루어짐
    Udp.endPacket();
  }
}
