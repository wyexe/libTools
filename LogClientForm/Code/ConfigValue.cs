using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Log.Code
{
    public class CConfigValue
    {
        // auto roll
        public static bool bDgvRoll = true;

        // Max dgv Count
        public static int nMaxLogCount = 10000;

        // Is Confirm MessageBox
        public static bool bConfirmMsgBox = false;

        // Log Color
        public static Color DebugColor = Color.White;

        public static Color WarningColor = Color.White;

        public static Color ExceptionColor = Color.White;

        public static Color CustomeColor = Color.White;

        public static List<CLogContent> ListClientLog = new List<CLogContent>();

        public static List<CLogContent> ListDgvLog = new List<CLogContent>();

        public static List<string> ListClientName = new List<string>();

        public static int nLogCount = 0;
        public static int nWarningCount = 0;
        public static int nExceptionCount = 0;

        public static int nProgressValue = 0;

        public static bool bAbandon = false;

        public static void AddClientName(string strClientName)
        {
            lock (ListClientName)
            {
                if (strClientName.Trim() != "" && ListClientName.Where((strClientName_) => { return strClientName == strClientName_; }).Count() == 0)
                    ListClientName.Add(strClientName);
            }
        }
    }
}
