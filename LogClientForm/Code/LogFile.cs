using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Log.Code
{
    public class CLogFile
    {
        public async Task<List<CLogContent>> GetLogFile(string strFilePath)
        {
            return await Task.Run<List<CLogContent>>(() =>
            {
                return ConvertToList(GetFileText(strFilePath));
            });
        }

        public string SaveFile(List<CLogContent> lst)
        {
            var strPath = System.Environment.CurrentDirectory;
            strPath += "\\" + DateTime.Now.ToString("yyyy-MM-dd HH.mm.ss");
            strPath += ".log";
            using (var Sw = new StreamWriter(strPath, false, Encoding.Unicode))
            {
                lst.ForEach((CLogContent LogContent) =>
                {
                    var strText = string.Empty;
                    strText += "#Stack:";
                    for (int i = 0; i < LogContent.ListStack.Count; i++)
                        strText += LogContent.ListStack[i] + "->";
                    strText += "\r\n";
                    strText += "#Time:" + LogContent.strTime + " ";
                    strText += "#Client:" + LogContent.strClientName + " ";
                    strText += "#Level:" + (int)LogContent.emLogType + " ";
                    strText += "#File:" + LogContent.strFileName + " ";
                    strText += "#FunName:" + LogContent.strFunName + " Line:" + LogContent.nLine.ToString() + " ";
                    strText += "#Content:" + LogContent.strLogContent + "\r\n";
                    Sw.WriteLine(strText);
                });
                Sw.Close();
            }
            return strPath;
        }

        private List<string> GetFileText(string strFilePath)
        {
            var ListFileText = new List<string>();

            using (StreamReader Sr = new StreamReader(strFilePath, Encoding.Unicode))
            {
                string strText = string.Empty;
                while ((strText = Sr.ReadLine()) != null)
                {
                    if (strText.Trim() != "" && strText.Trim().IndexOf("#    Build") == -1)
                        ListFileText.Add(strText);
                }
                Sr.Close();
            }

            return ListFileText;
        }

        private static List<string> GetStackList_By_StrText(string strText)
        {
            // #Stack:4789240->4789240->4789240->4789240->4789240->4789240->4789240->4789240->4789240->4789240->
            var nIndex = strText.IndexOf("#Stack:");
            if (nIndex != 0)
            {
                //MessageBox.Show("Stack Invalid format! Text:" + strText);;
                throw new Exception("Invalid format!");
            }
            strText = strText.Substring(nIndex + "#Stack:".Length);
            return strText.Split(new string[] { "->" }, StringSplitOptions.RemoveEmptyEntries).ToList();
        }

        private static string GetLogContentByTextType(ref string strText, string strType)
        {
            var nIndex = strText.IndexOf(strType);
            if (nIndex == -1)
            {
                MessageBox.Show("LogContent Invalid format! Text:" + strText);
                return "";
                //throw new Exception("Invalid format!");
            }

            // 11:11:11 111
            strText = strText.Substring(nIndex + strType.Length);
            nIndex = strText.IndexOf("#");
            if (nIndex == -1)
                return strText.Trim();

            var str = strText.Substring(0, nIndex).Trim();
            strText = strText.Substring(nIndex);
            return str;
        }

        private static bool GetLogContentByText(string strText, ref CLogContent LogContent)
        {
            //#Time:11:11:11 111	#Level:1	#File:PlayerTransaction.cpp:3658	#FunName:CLoginPlayerExtend::SetLoginPlayerIndex	#Content:我是仓库号:asdasdasd存在合适的小号:basdqweqwe! 准备交易!	
            LogContent.strTime = GetLogContentByTextType(ref strText, "#Time:");
            if (LogContent.strTime.Length == 0)
                return false;


            LogContent.strClientName = GetLogContentByTextType(ref strText, "#Client");
            if (LogContent.strClientName.Length != 0 && LogContent.strClientName[0] == ':')
                LogContent.strClientName.Remove(0, 1);

            Code.CConfigValue.AddClientName(LogContent.strClientName);

            int nValue = 0;
            string strLogType = GetLogContentByTextType(ref strText, "#Level:");
            if (int.TryParse(strLogType, out nValue))
                LogContent.emLogType = (em_Log_Type)nValue; 
            else
            {
                switch (strLogType)
                {
                    case "普通":
                        LogContent.emLogType = em_Log_Type.em_Log_Type_Debug;
                        break;
                    case "函数":
                        LogContent.emLogType = em_Log_Type.em_Log_Type_Custome;
                        break;
                    case "异常":
                        LogContent.emLogType = em_Log_Type.em_Log_Type_Exception;
                        break;
                    default:
                        LogContent.emLogType = em_Log_Type.em_Log_Type_Debug;
                        break;
                }
            }
            
            LogContent.strFileName = GetLogContentByTextType(ref strText, "#File:");
            LogContent.strFunName = GetLogContentByTextType(ref strText, "#FunName:");
            LogContent.strLogContent = GetLogContentByTextType(ref strText, "#Content:");
            return true;
        }

        private static List<CLogContent> ConvertToList(List<string> lst)
        {
            var vList = new List<CLogContent>();
            int nMaxCount = lst.Count / 2;
            int nIndex = 0;
            while (lst.Count >= 2)
            {
                var LogContent = new CLogContent();
                try
                {
                    LogContent.ListStack = GetStackList_By_StrText(lst[0]);
                    if (GetLogContentByText(lst[1], ref LogContent))
                        vList.Add(LogContent);

                    lst.RemoveAt(0);
                    lst.RemoveAt(0);
                }
                catch(Exception)
                {
                    lst.RemoveAt(0);
                }

               
                Code.CConfigValue.nProgressValue = (int)(((float)(float)(nIndex++) / (float)nMaxCount) * 100);

            }
            Code.CConfigValue.nProgressValue = 100;
            return vList;
        }
    }
}
