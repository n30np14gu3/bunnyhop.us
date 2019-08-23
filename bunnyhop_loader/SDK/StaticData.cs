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
            {"server", "Server error, try again later. Code: #f32fes" },
            {"software", "Program error, try again later. Code: #f32522" },
            {"loginNotFound", "This user doesn’t exist. Code: #f64364" },
            {"updateHwid", "Your account is linked to another PC. Unlink your account in the profile settings. Code: # f13236" },
            {"duplicateHwid", "You already have an account that is linked to this PC. Code: #f66666" },
            {"empty_paramers", "Type your login!" },
            {"can_not_connection", "Program error, try again later. Code: #f32522" },
            {"versionNotFound","Server error, try again later. Code: #f23543" }
        };

        public static readonly string UserInfoText = "If you have any problems with the launch, try to find a solution in the FAQ on our website.";

        public static readonly string AdLable = "DISABLE ADVERTISING";
        public static readonly string AdText = "Disable advertising in clan tag «bunnyhop.us» and get three additional skins.";
    }
}
