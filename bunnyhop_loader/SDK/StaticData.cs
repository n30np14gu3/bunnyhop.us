using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bunnyhop_loader.SDK
{
    class StaticData
    {
        public static readonly Dictionary<string, string> ErrorCodes = new Dictionary<string, string>
        {
            {"server", "ошибка на сервере" },
            {"software", "не передан login или hwid" },
            {"loginNotFound", "ошибка на сервере при обновление hwid пользователя" },
            {"updateHwid", "такой hwid уже кто то использует" },
            {"duplicateHwid", "ошибка на сервере при поиске hwid в бд" },
            {"empty_paramers", "введите логин!" },
            {"can_not_connection", "Не удалось соедениться с сервером!" }
        };

        public static readonly string UserInfoText = "If you have any problems with the launch, try to find a solution in the FAQ on our website.";

        public static readonly string AdLable = "DISABLE ADVERTISING";
        public static readonly string AdText = "Disalbe advertising in clan tab and get three additional skins.";
    }
}
