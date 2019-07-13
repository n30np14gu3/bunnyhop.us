using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

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

                socket.Send(BitConverter.GetBytes(client_token.Count));

                for (int i = 0; i < client_token.Count; i++)
                    client_token[i] ^= client_key[i % 4];

                socket.Send(client_token.ToArray());

                for (int i = 0; i < client_token.Count; i++)
                    client_token[i] ^= client_key[i % 4 + 4];

                List<byte> token_hash = Encoding.ASCII.GetBytes(HWID.Sha256(client_token.ToArray())).ToList();
                token_hash.Add(0);
                socket.Send(token_hash.ToArray());

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
