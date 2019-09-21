using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Log.Code
{
    public class API
    {
        [DllImport("LogDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int WriteMsgToClient(string wsClientName, string wsContent, StringBuilder sbErrMsg);

        [DllImport("LogDLL.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Release();
    }
}
