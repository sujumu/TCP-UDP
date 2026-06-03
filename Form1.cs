using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace example350_1
{
    public partial class Form1 : Form
    {
        UdpClient udpClient = new UdpClient(60001);
        Thread t;

        bool is_udp_run = false;

        public Form1()
        {
            InitializeComponent();
        }


        void nockanda_udp()
        {
            try
            {

                Byte[] sendBytes = Encoding.UTF8.GetBytes("나는 C# UDP클라이언트이다!");

                



                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);

                DateTime dt = DateTime.Now;
                while (is_udp_run)
                {
                    if((DateTime.Now - dt).TotalSeconds > 1)
                    {
                        dt = DateTime.Now;
                        //1초마다 한번씩 전송한다
                        udpClient.Connect("192.168.219.109", 60000);
                        udpClient.Send(sendBytes, sendBytes.Length);
                    }
                   


                    // Blocks until a message returns on this socket from a remote host.
                    Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                    string returnData = Encoding.UTF8.GetString(receiveBytes);
                    /*
                    // Uses the IPEndPoint object to determine which of these two hosts responded.
                    Console.WriteLine("This is the message you received " +
                                                 returnData.ToString());
                    Console.WriteLine("This message was sent from " +
                                                RemoteIpEndPoint.Address.ToString() +
                                                " on their port number " +
                                                RemoteIpEndPoint.Port.ToString());
                    */
                    richTextBox1.Text += returnData + "@" + RemoteIpEndPoint.Address.ToString() + ":" + RemoteIpEndPoint.Port.ToString() + "\n";
                }
                udpClient.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            is_udp_run = true;
            t = new Thread(new ThreadStart(nockanda_udp));
            t.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            is_udp_run = false;
            if (t != null && t.IsAlive) t.Abort();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
