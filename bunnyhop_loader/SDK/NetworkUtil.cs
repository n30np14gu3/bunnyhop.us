using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using xNet;

namespace bunnyhop_loader.SDK
{
    public class NetworkUtil
    {
        public static (bool, string) Auth(string login)
        {
            if (string.IsNullOrWhiteSpace(login))
                return (false, "empty_paramers");

            try
            {
                using (HttpRequest request = new HttpRequest() { IgnoreProtocolErrors = true })
                {
                    RequestParams data = new RequestParams();
                    data["login"] = login;
                    data["hwid"] = HWID.GetSign();

                    string rsp = request.Post("https://bunnyhop.us/api/software/auth", data).ToString();

                    ProgramData.UserInfo = JsonConvert.DeserializeObject<UserData>(rsp);
                    if(ProgramData.UserInfo.status == 3)
                        return (false, ProgramData.UserInfo.error[0]);

                    if(string.IsNullOrWhiteSpace(Properties.Settings.Default.login))
                    {
                        Properties.Settings.Default.login = login;
                        Properties.Settings.Default.Save();
                    }

                    return (true, "");
                    
                }
            }
            catch(HttpException ex)
            {
                Clipboard.SetText(ex.Message);
                return (false, "can_not_connection");
            }
            catch (Exception ex)
            {
                Clipboard.SetText(ex.Message);
                return (false, "server");
            }
        }
    }
}
