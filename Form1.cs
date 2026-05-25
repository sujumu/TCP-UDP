using System;
using System.Net.Sockets;   // TCP 통신용
using System.Text;          // 문자열 ↔ byte 변환용
using System.Threading;     // Thread 사용
using System.Windows.Forms; // WinForms 사용

namespace example348_1
{
    public partial class Form1 : Form
    {
        TcpClient client;          // TCP 서버에 접속할 클라이언트 객체
        Thread t;                  // TCP 통신을 실행할 별도 스레드
        bool is_tcp_run = false;   // TCP 통신 실행 여부

        public Form1()
        {
            InitializeComponent(); // Form 화면 초기화
        }

        void nockanda()
        {
            try
            {
                // TCP 실행 상태가 true인 동안 계속 동작
                while (is_tcp_run)
                {
                    // ESP8266 TCP 서버에 접속
                    // IP 주소는 ESP8266 시리얼 모니터에 출력된 IP로 변경해야 함
                    client = new TcpClient("192.168.0.12", 60000);

                    // 서버로 보낼 문자열을 byte 배열로 변환
                    // \n은 ESP8266이 한 줄 단위로 읽기 위한 구분자
                    Byte[] data = Encoding.UTF8.GetBytes("클라이언트가 서버에게 보내는 메시지!\n");

                    // TCP 데이터 송수신 통로 생성
                    NetworkStream stream = client.GetStream();

                    // 연결 성공 메시지 출력
                    richTextBox1.Text += "서버와 연결되었다\n";

                    // 서버에서 받은 데이터를 저장할 변수와 버퍼
                    String responseData = String.Empty;
                    Byte[] recv = new Byte[256];

                    // 1초마다 데이터 전송하기 위한 시간 저장
                    DateTime dt = DateTime.Now;

                    // TCP가 실행 중이고 서버와 연결되어 있는 동안 반복
                    while (is_tcp_run && client.Connected)
                    {
                        // 서버에서 보낸 데이터가 있으면 수신
                        if (stream.DataAvailable)
                        {
                            // 서버가 보낸 byte 데이터를 recv 배열에 저장
                            Int32 bytes = stream.Read(recv, 0, recv.Length);

                            // 받은 byte 데이터를 문자열로 변환
                            responseData = Encoding.UTF8.GetString(recv, 0, bytes);

                            // 받은 문자열을 화면에 출력
                            richTextBox1.Text += responseData + "\n";
                        }

                        // 1초마다 서버로 데이터 전송
                        if ((DateTime.Now - dt).TotalSeconds > 1)
                        {
                            dt = DateTime.Now;

                            // 서버로 byte 데이터 전송
                            stream.Write(data, 0, data.Length);

                            // 전송 로그 출력
                            richTextBox1.Text += "데이터를 전송했다!\n";
                        }
                    }

                    // 연결 종료
                    stream.Close();
                    client.Close();
                }
            }
            catch (ArgumentNullException e)
            {
                // IP 주소 등이 null일 때 발생할 수 있는 오류
            }
            catch (SocketException e)
            {
                // 서버 접속 실패, IP 오류, 포트 오류 등 TCP 관련 오류
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // TCP 통신 시작
            is_tcp_run = true;

            // nockanda 함수를 별도 스레드에서 실행
            // 화면 멈춤을 방지하기 위함
            t = new Thread(new ThreadStart(nockanda));
            t.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // TCP 통신 종료
            is_tcp_run = false;

            // 서버와 연결되어 있으면 연결 종료
            if (client.Connected) client.Close();

            // 스레드가 실행 중이면 강제 종료
            if (t.IsAlive) t.Abort();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Form이 처음 실행될 때 동작하는 부분
        }
    }
}
