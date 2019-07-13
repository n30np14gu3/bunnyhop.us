using bunnyhop_loader.SDK;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
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

        private byte[] _dll = null;
        private int _csgoPid;

        private bool _injected;
        private bool _isEnabled;
        private bool _status;

        private Thread _checkcsgo = null;

        public MainWindow()
        {
            InitializeComponent();
            BActionBtn.IsEnabled = false;
            BLogout.IsEnabled = false;
        }

        private void DragHeader_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _isDrag = true;
            DragMove();
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

            if (UnixTimeStampToDateTime(ProgramData.UserInfo.version) > ProgramData.LastUpdate)
            {
                updateRequested();
                return;
            }

            IntPtr hwnd = WinApi.FindWindowA(IntPtr.Zero, "Counter-Strike: Global Offensive");
            if (hwnd != IntPtr.Zero)
            {
                IntPtr lResult = WinApi.SendMessage(hwnd, 0, IntPtr.Zero, (IntPtr)0x103);
                if (lResult == (IntPtr)0x505)
                {
                    _injected = true;
                    _isEnabled = true;
                    BActionBtn.Content = "Turn off bunnyhop";
                    return;
                }

                if (lResult == (IntPtr)0x504)
                {
                    _injected = true;
                    _isEnabled = false;
                    BActionBtn.Content = "Turn on bunnyhop";
                }

                BActionBtn.IsEnabled = true;
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
            InfoMessage.Tag = url;
            InfoMessage_Text.Tag = actionType;
            InfoMessage_Text.ActionContent = actionText;
            InfoMessage_Text.Content = message;
            InfoMessage.IsActive = true;
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
                            TOnline.Content = $"ONLINE: {ping.data.online}";
                        });
                        break;

                    case "update":
                        WsResponce<UpdateStruct> update = obj.ToObject<WsResponce<UpdateStruct>>();
                        if (UnixTimeStampToDateTime(update.data.version) > ProgramData.LastUpdate)
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
            initOnline();
            autoAuth();
        }

        private async void BActionBtn_Click(object sender, RoutedEventArgs e)
        {
            _checkcsgo?.Abort();

            if (!_injected)
            {
                BActionBtn.IsEnabled = false;
                showMessage("Происходит запуск, подождите...", "Закрыть", "");
                await Task.Run(() => runCsGo());
#if !DEBUG
                await Task.Run(() => downloadLibs());
                if(!_status)
                    return;
#else
                _status = true;
                _dll = File.ReadAllBytes("test.dll");
#endif
                await Task.Run(() => injectLibs());
                if (!_status)
                    return;
                await Task.Run(() => signCheat());
                if (!_status)
                    return;

                BActionBtn.IsEnabled = false;
                _injected = true;
                BActionBtn.IsEnabled = true;
                _isEnabled = true;
                BActionBtn.Content = "Выключить";
                _checkcsgo = new Thread(validation) { IsBackground = true, Priority = ThreadPriority.Lowest };
                _checkcsgo.Start();
                showMessage("bunnyhop успешно запущен!", "Закрыть", "");
            }
            else
            {
                _isEnabled = !_isEnabled;


                IntPtr hwnd = WinApi.FindWindowA(IntPtr.Zero, "Counter-Strike: Global Offensive");
                if (hwnd != IntPtr.Zero)
                {
                    BActionBtn.Content = _isEnabled ? "Turn off bunnyhop" : "Turn on bunnyhop";
                    IntPtr comand = _isEnabled ? (IntPtr)0x102 : (IntPtr)0x101;
                    WinApi.SendMessage(hwnd, 0, IntPtr.Zero, comand);
                    showMessage($"Вы {(_isEnabled ? "включили" : "выключили")} bunnyhpop", "Закрыть", "");
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
                        Dispatcher.Invoke(() => { BActionBtn.Content = "Run Bunnyhop"; });
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
                    throw new Exception("Не удалось запустить bunnyhop! Попробуйте ещё раз...");
            }
            catch (Exception ex)
            {
                showMessage(ex.Message, "Закрыть", "action_close");
                _status = false;
            }
            finally
            {
                Array.Clear(_dll, 0, _dll.Length);
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

                    _dll = request.Post("https://bunnyhop.us/api/application/request_dll", data).ToBytes();
                    if (_dll.Length == 0)
                        throw new Exception("");

                    _status = true;
                }
            }
            catch
            {
                showMessage("Ошибка подключения. Попробуйте ещё раз...", "Закрыть", "action_close");
                _status = false;
            }
        }

        void updateRequested()
        {
#if !DEBUG
            showMessage("Вышло новое обновление! Загрузите новую версию с нашего сайта.", "Загрузить", "open_url", "https://bunnyhop.us");
            BLogin.IsEnabled = false;
            BActionBtn.IsEnabled = false;
#else
            BActionBtn.IsEnabled = true;
            return;
#endif
        }
    }
}
