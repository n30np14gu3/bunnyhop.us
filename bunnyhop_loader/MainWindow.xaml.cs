using bunnyhop_loader.SDK;
using Newtonsoft.Json.Linq;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using WebSocketSharp;
using xNet;

namespace bunnyhop_loader
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool _isDrag;
        private bool _isLinked;

        private string _dll = "";
        private int _csgoPid;

        private bool _injected;
        private bool _isEnabled;
        private bool _status;

        private Thread _checkcsgo = null;
        private Thread _checkSub = null;

        public MainWindow()
        {
            InitializeComponent();
            BActionBtn.IsEnabled = false;
            BLogout.IsEnabled = false;
        }

        private void DragHeader_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if(_isLinked)
                return;

            _isDrag = true;
            try
            {
                DragMove();
            }
            catch
            {
                //nothing
            }
        }

        private void DragHeader_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _isDrag = false;
        }

        private void DragHeader_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isDrag && WindowState == WindowState.Maximized)
            {
                _isDrag = false;
                WindowState = WindowState.Normal;
            }
        }

        private void CloseBtn_Click(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void MinBtn_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void InfoMessage_Text_ActionClick(object sender, RoutedEventArgs e)
        {
            if (InfoMessage_Text.Tag.ToString() == "open_url")
            {
                Process.Start(InfoMessage.Tag.ToString());
                return;
            }

            InfoMessage.IsActive = false;
        }

        private void MainForm_Activated(object sender, EventArgs e)
        {

        }

        void autoAuth()
        {
            if (string.IsNullOrWhiteSpace(Properties.Settings.Default.login))
                return;

            TLogin.Text = Properties.Settings.Default.login;
            auth();
        }

        void auth()
        {
            MainForm.IsEnabled = false;
            var result = NetworkUtil.Auth(TLogin.Text);
            MainForm.IsEnabled = true;
            if (!result.Item1)
            {
                showMessage(StaticData.ErrorCodes[result.Item2], "ОК", "error_msg");
                return;
            }

            if (ProgramData.UserInfo.version != ProgramData.LastUpdate)
            {

                updateRequested();
                return;
            }

            LoginInfoLabel.Content = TLogin.Text;
            LoginInfoText.Text = StaticData.UserInfoText;

            AdsInfoLabel.Content = StaticData.AdLable;
            AdsInfoText.Text = StaticData.AdText;

            TLogin.Visibility = Visibility.Hidden;

            BLogin.IsEnabled = false;
            BLogin.Visibility = Visibility.Hidden;

            BLogout.IsEnabled = true;
            BLogout.Visibility = Visibility.Visible;
            BActionBtn.Visibility = Visibility.Visible;

            if (ProgramData.UserInfo.subscribe)
            {
                BShowWebsite.Content = UnixTimeStampToDateTime(ProgramData.UserInfo.dateEnd);
            }

            BActionBtn.IsEnabled = true;
            _checkSub = new Thread(checkSub) {IsBackground = true, Priority = ThreadPriority.Lowest};
            _checkSub.Start();
            IntPtr hwnd = WinApi.FindWindowA(IntPtr.Zero, "Counter-Strike: Global Offensive");
            if (hwnd != IntPtr.Zero)
            {
                IntPtr lResult = WinApi.SendMessage(hwnd, 0, IntPtr.Zero, (IntPtr)0x103);
                if (lResult == (IntPtr)0x605)
                {
                    _injected = true;
                    _isEnabled = true;
                    BActionBtn.Content = "Turn off bunnyhop";
                    return;
                }

                if (lResult == (IntPtr)0x604)
                {
                    _injected = true;
                    _isEnabled = false;
                    BActionBtn.Content = "Turn on bunnyhop";
                }
            }


        }

        private void checkSub()
        {
            while (true)
            {
                if (UnixTimeStampToDateTime(ProgramData.UserInfo.dateEnd) < DateTime.Now)
                {
                    Dispatcher.Invoke(() =>
                    {
                        BShowWebsite.Content = "Move to our website";
                        
                    });
                    break;
                }
                Thread.Sleep(1000);
            }
        }
        private DateTime UnixTimeStampToDateTime(double unixTimeStamp)
        {
            DateTime dtDateTime = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
            dtDateTime = dtDateTime.AddSeconds(unixTimeStamp).ToLocalTime();
            return dtDateTime;
        }

        private void showMessage(string message, string actionText, string actionType, string url = "")
        {
            //InfoMessage.Background = new SolidColorBrush(backColor);
            Dispatcher.Invoke(() =>
            {
                InfoMessage.Tag = url;
                InfoMessage_Text.Tag = actionType;
                InfoMessage_Text.ActionContent = actionText;
                InfoMessage_Text.Content = message;
                InfoMessage.IsActive = true;
            });
        }

        private void BLogin_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrWhiteSpace(TLogin.Text))
                return;

            auth();
        }

        private void BLogout_Click(object sender, RoutedEventArgs e)
        {
            Properties.Settings.Default.Reset();
            Environment.Exit(0);
        }

        private async void initOnline()
        {
            using (WebSocket ws = new WebSocket("wss://bunnyhop.us/api/software/"))
            {
                ws.OnMessage += Ws_OnMessage;
                ws.OnError += Ws_OnError;
                ws.Connect();
                if (!ws.IsAlive)
                    return;
                await Task.Delay(-1);
            }
        }

        private void Ws_OnMessage(object sender, MessageEventArgs e)
        {
            try
            {
                JObject obj = JObject.Parse(e.Data);
                switch (obj["type"].ToString())
                {
                    case "online":
                        WsResponce<PingStruct> ping = obj.ToObject<WsResponce<PingStruct>>();
                        Dispatcher.Invoke(() => 
                        {
                            TOnline.Content = $"{ping.data.online}";
                        });
                        break;

                    case "update":
                        WsResponce<UpdateStruct> update = obj.ToObject<WsResponce<UpdateStruct>>();
                        if (update.data.version != ProgramData.LastUpdate)
                            Dispatcher.Invoke(() => 
                            {
                                updateRequested();
                            });
                        break;

                }
            }
            catch
            {
                //nothing
            }
        }

        private void Ws_OnError(object sender, WebSocketSharp.ErrorEventArgs e)
        {
            showMessage(e.Message, "OK", "error_msg");
        }

        private void MainForm_Loaded(object sender, RoutedEventArgs e)
        {
            Keyboard.Focus(TLogin);
            initOnline();
            autoAuth();
        }

        private async void BActionBtn_Click(object sender, RoutedEventArgs e)
        {
            _checkcsgo?.Abort();

            if (!_injected)
            {
                BActionBtn.IsEnabled = false;
                showMessage("Starting up, wait couple of seconds...", "Close", "");
                await Task.Run(() => runCsGo());
#if !DEBUG
                await Task.Run(() => downloadLibs());
                if(!_status)
                    return;
#else
                if (MessageBox.Show("Use CE?", "", MessageBoxButton.YesNo) == MessageBoxResult.No)
                {
                    _status = true;
                    _dll = new FileInfo("test.dll").FullName;
                }
                else
                {
                    MessageBox.Show("Press OK after Inject", "");
                    goto inject_skip;
                }

#endif
                await Task.Run(() => injectLibs());
                if (!_status)
                    return;

#if DEBUG
                inject_skip:
#endif
                await Task.Run(() => signCheat());
                if (!_status)
                    return;

                BActionBtn.IsEnabled = false;
                _injected = true;
                BActionBtn.IsEnabled = true;
                _isEnabled = true;
                BActionBtn.Content = "Turn off bunnyhop";
                _checkcsgo = new Thread(validation) { IsBackground = true, Priority = ThreadPriority.Lowest };
                _checkcsgo.Start();
                showMessage("Bunnyhop launched successful!", "Close", "");
            }
            else
            {
                _isEnabled = !_isEnabled;


                IntPtr hwnd = WinApi.FindWindowA(IntPtr.Zero, "Counter-Strike: Global Offensive");
                if (hwnd != IntPtr.Zero)
                {
                    BActionBtn.Content = _isEnabled ? "Turn off bunnyhop" : "Turn on bunnyhop";
                    IntPtr comand = _isEnabled ? (IntPtr)0x202 : (IntPtr)0x201;
                    WinApi.SendMessage(hwnd, 0, IntPtr.Zero, comand);
                    showMessage($"You have turned {(_isEnabled ? "on" : "off")} bunnyhop", "Close", "");
                }
                else
                {
                    _isEnabled = false;
                    _injected = false;
                }
            }
        }

        private void validation()
        {
            try
            {
                while (true)
                {
                    IntPtr check = WinApi.FindWindowA(IntPtr.Zero, "Counter-Strike: Global Offensive");
                    if (check == IntPtr.Zero)
                    {
                        _isEnabled = false;
                        _injected = false;
                        _status = false;
                        if (Dispatcher != null) Dispatcher.Invoke(() => { BActionBtn.Content = "Run Bunnyhop"; });
                        _checkcsgo?.Abort();
                        _checkcsgo = null;
                        break;
                    }

                    Thread.Sleep(500);
                }
            }
            catch
            {
                //
            }
        }

        private void signCheat()
        {
            _status = CheatVerification.Verify();
        }

        void injectLibs()
        {
            try
            {
                if (!Injector.ManualMapInject(_csgoPid, _dll))
                    throw new Exception("There was trouble with launching bunnyhop. Try again...");
            }
            catch (Exception ex)
            {
                showMessage(ex.Message, "Close", "action_close");
                _status = false;
            }
            finally
            {
                _dll = "";
            }
        }


        void runCsGo()
        {
        repeat:
            try
            {
                if (Process.GetProcessesByName("csgo").Length == 0)
                {
                    Process.Start("steam://rungameid/730");
                    while (Process.GetProcessesByName("csgo").Length == 0)
                    {
                    }
                }

                Thread.Sleep(6000);
            kek:
                int loadedModules = 0;
                foreach (ProcessModule module in Process.GetProcessesByName("csgo")[0].Modules)
                {
                    if (module.ModuleName == "client_panorama.dll")
                        loadedModules++;

                    if (module.ModuleName == "engine.dll")
                        loadedModules++;

                    if (module.ModuleName == "server.dll")
                        loadedModules++;
                }

                if (loadedModules != 3)
                    goto kek;

                Thread.Sleep(2000);
                _csgoPid = Process.GetProcessesByName("csgo")[0].Id;
            }
            catch
            {
                goto repeat;
            }
        }


        void downloadLibs()
        {
            try
            {
                using (HttpRequest request = new HttpRequest { IgnoreProtocolErrors = true })
                {
                    RequestParams data = new RequestParams();
                    data["hwid"] = ProgramData.Hwid;

                    string tmpFile = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName() + ".dll");
                    byte[] tmp = request.Post("https://bunnyhop.us/api/software/request_dll", data).ToBytes();
                    if(tmp.Length == 0)
                        throw new Exception("");


                    _dll = tmpFile;

                    
                    if (_dll.Length == 0)
                        throw new Exception("");

                    File.WriteAllBytes(tmpFile, tmp);
                    Array.Clear(tmp, 0, tmp.Length);
                    _status = true;
                }
            }
            catch
            {
                showMessage("Program error, try again later. Code: #f32522", "Close", "action_close");
                _status = false;
            }
        }

        void updateRequested()
        {
#if !DEBUG
            showMessage("New update released! Download the new version from our website.", "Download", "open_url", "https://bunnyhop.us");
            BLogin.IsEnabled = false;
            BActionBtn.IsEnabled = false;
#else
            BActionBtn.IsEnabled = true;
#endif
        }

        private void BShowWebsite_Click(object sender, RoutedEventArgs e)
        {
            Process.Start("https://bunnyhop.us");
        }

        private void Label_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _isLinked = true;
            Process.Start("https://bunnyhop.us");
            _isLinked = false;
        }
    }
}
