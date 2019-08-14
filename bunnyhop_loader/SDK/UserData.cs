using System;

namespace bunnyhop_loader.SDK
{
    [Serializable]
    internal class UserData
    {
        public int status;
        public string token;
        public bool subscribe;
        public int dateEnd;
        public string[] error;
        public int version;
    }
}
