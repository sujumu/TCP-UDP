using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;

namespace example349_1
{
    public partial class Form1 : Form
    {
        bool is_server_run = false;
        Thread t;

        TcpClient client;
        NetworkStream stream;
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            is_server_run = true;
            t = new Thread(new ThreadStart(nockanda_tcp_server));
            t.Start();
        }

        class Nockada
        {
            //{"device":"servo","d_id":1,"value":90}
            public string device;
            public int d_id;
            public int value;
        }

        void nockanda_tcp_server()
        {
            TcpListener server = null;
            try
            {
                //서버의 포트번호이다!
                Int32 port = 60000;
                //서버의 ip주소이다!
                IPAddress localAddr = IPAddress.Parse("192.168.219.102");

                //C#에서 서버의 인스턴스가 여기서 만들어진다!(아직 서버가 작동된건 아님)
                server = new TcpListener(localAddr, port);

                //서버가 시작되었다!
                server.Start();

                //서버가 클라이언트를 기다리는 작업을 컨츄롤하는 루프
                while (is_server_run)
                {
                    richTextBox1.Text += "서버가 클라이언트의 접속을 기다린다!\n";

                    //서버가 클라이언트의 접속을 기다리는 부분
                    client = server.AcceptTcpClient();

                    richTextBox1.Text += "클라이언트가 나타났다!\n";

                    //클라이언트하고 데이터를 주고받기 위한 스트림을 생성
                    stream = client.GetStream();
                    StreamReader sr = new StreamReader(stream);
                    //StreamWriter sw = new StreamWriter(stream);


                    //서버와 클라이언트 접속을 유지하면서 양방향통신을 한다
                    //언제까지? 접속이 유지되어 있을때 까지
                    DateTime dt = DateTime.Now;
                    DateTime timeout = DateTime.Now;
                    while (is_server_run && client.Connected)
                    {
                        //클라이언트에서 서버에 전송한 메시지가 있냐?
                        if (stream.DataAvailable)
                        {
                            //메시지가 있다!
                            string msg = sr.ReadLine();
                            msg = msg.Replace("\r", "");
                            msg = msg.Replace("\n", "");
                            if (msg == "연결!")
                            {
                                //richTextBox1.Text += "조건에 걸렸다\n";
                                //msg라는 문자열에 "연결!"패턴이 존재하느냐?
                                //클라이언트에게 마지막으로 받은 메시지의 시간
                                timeout = DateTime.Now;
                            }
                            else
                            {
                                //JSON
                                JObject data = JObject.Parse(msg);//역직렬화
                                //data["device"]
                                //data["msg"]
                                //{ "device":"servo","d_id":1,"value":90}
                                label1.Text = data["value"].ToString();
                                //richTextBox1.Text += "다바이스ID=" + data["device"] + ", 메시지=" + data["msg"] + "\n";
                            }
                            //richTextBox1.Text += msg + "\n";
                        }

                        
                        //서버가 클라이언트에게 뭔가 전송한다!
                        if ((DateTime.Now - dt).TotalSeconds > 1)
                        {
                            dt = DateTime.Now;

                            
                        }
                        

                        //서버에서 클라이언트의 작동상태를 점검한다
                        if((DateTime.Now - timeout).TotalSeconds > 3)
                        {
                            richTextBox1.Text += "클라이언트 연결 해제\n";
                            break;
                        }
                    }

                    // Shutdown and end connection
                    sr.Close();
                    stream.Close();
                    client.Close();
                }
            }
            catch (SocketException e)
            {
                //Console.WriteLine("SocketException: {0}", e);
            }
            finally
            {
                // Stop listening for new clients.
                server.Stop();
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            is_server_run = false;
            if (t.IsAlive) t.Abort();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            if(client != null && client.Connected && stream != null)
            {
                Nockada n = new Nockada();
                n.device = "servo";
                n.d_id = 1;
                n.value = int.Parse(textBox1.Text);

                string output = JsonConvert.SerializeObject(n);
                output = output + "\n";
                //string msg = "나는 C# TCP 서버이다!!!\n";

                byte[] msg2byte = Encoding.UTF8.GetBytes(output);

                stream.Write(msg2byte, 0, msg2byte.Length);
            }
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            label2.Text = trackBar1.Value.ToString();

            if (client != null && client.Connected && stream != null)
            {
                Nockada n = new Nockada();
                n.device = "servo";
                n.d_id = 1;
                n.value = trackBar1.Value;

                string output = JsonConvert.SerializeObject(n);
                output = output + "\n";
                //string msg = "나는 C# TCP 서버이다!!!\n";

                byte[] msg2byte = Encoding.UTF8.GetBytes(output);

                stream.Write(msg2byte, 0, msg2byte.Length);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
