using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Windows;

namespace bunnyhop_loader.SDK
{

    class CheatVerification
    {
        public static bool Verify()
        {
            try
            {
                IPAddress localAddr = IPAddress.Parse("127.0.0.1");
                int port = 1548;
                TcpListener server = new TcpListener(localAddr, port);
                server.Start();


                Socket socket = server.AcceptSocket();

                byte[] client_key = new byte[8];
                socket.Receive(client_key);

                List<byte> client_token = Encoding.ASCII.GetBytes(ProgramData.UserInfo.token).ToList();
                client_token.Add(0);

#if DEBUG
                MessageBox.Show($"Login {ProgramData.Login}");
#endif
                List<byte> client_login = Encoding.ASCII.GetBytes(ProgramData.Login).ToList();
                client_login.Add(0);

#if DEBUG
                MessageBox.Show($"HWID {ProgramData.Hwid}");
#endif

                List<byte> client_hwid = Encoding.ASCII.GetBytes(ProgramData.Hwid).ToList();
                client_hwid.Add(0);

                for (int i = 0; i < client_token.Count; i++)
                    client_token[i] ^= client_key[i % 4];

                for (int i = 0; i < client_login.Count; i++)
                    client_login[i] ^= client_key[i % 4];

                for (int i = 0; i < client_hwid.Count; i++)
                    client_hwid[i] ^= client_key[i % 4];

                socket.Send(BitConverter.GetBytes(client_token.Count));
                socket.Send(client_token.ToArray());

                for (int i = 0; i < client_token.Count; i++)
                    client_token[i] ^= client_key[i % 4 + 4];

                socket.Send(BitConverter.GetBytes(client_login.Count));
                socket.Send(client_login.ToArray());

                socket.Send(BitConverter.GetBytes(client_hwid.Count));
                socket.Send(client_hwid.ToArray());

                List<byte> token_hash = Encoding.ASCII.GetBytes(HWID.Sha256(client_token.ToArray()).ToUpper()).ToList();
                token_hash.Add(0);
                socket.Send(token_hash.ToArray());

                //stub
                byte[] end_header = new byte[4];
                socket.Receive(end_header);

                socket.Close();
                server.Stop();

                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
