# TCP/UDP 통신 프로젝트
ESP8266과 C# WinForms를 활용하여 TCP/UDP 통신을 실습한 프로젝트입니다.

## 프로젝트 개요
```text
C# WinForms 프로그램
        │
        │ TCP/IP
        ▼
공유기 또는 무선 공유기
        │
        │ Wi-Fi
        ▼
Wemos D1 R1 / ESP8266
        │
        │ GPIO 제어
        ▼
LED 또는 센서
```
### 프로젝트 구성
#1 ESP8266을 네트워크 통신 보드로 사용하고, C# WinForms 프로그램을 PC 클라이언트로 구성하여 TCP/UDP 데이터 송수신을 구현
