using System;


namespace bunnyhop_loader.SDK
{
    [Serializable]
    class WsResponce<T>
    {
        public string type;
        public T data;
    }
}
