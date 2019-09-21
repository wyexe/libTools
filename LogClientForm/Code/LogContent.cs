using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace Log.Code
{
    [Serializable]
    public enum em_Log_Type
    {
        em_Log_Type_Invalid,
        em_Log_Type_Debug,
        em_Log_Type_Warning,
        em_Log_Type_Custome,
        em_Log_Type_Exception,
    }
    [Serializable]
    public class CLogContent
    {
        
        private string strLogContent_ = string.Empty;

        private List<string> ListStack_ = new List<string>();

        private string strFunName_ = string.Empty;

        private int nLine_ = 0;

        private string strFileName_ = string.Empty;

        private em_Log_Type emLogType_ = em_Log_Type.em_Log_Type_Debug;

        private string strTime_ = string.Empty;

        private string strClientName_ = string.Empty;

        private int nRepeatCount_ = 0;

        public int nRepeatCount
        {
            get { return nRepeatCount_; }
            set { nRepeatCount_ = value; }
        }

        public string strClientName
        {
            get { return strClientName_; }
            set { strClientName_ = value; }
        }

        public int nLine
        {
            get { return nLine_; }
            set { nLine_ = value; }
        }

        public string strFileName
        {
            get { return strFileName_; }
            set { strFileName_ = value; }
        }

        public em_Log_Type emLogType
        {
            get { return emLogType_; }
            set { emLogType_ = value; }
        }

        public string strTime
        {
            get { return strTime_; }
            set { strTime_ = value; }
        }

        public string strFunName
        {
            get { return strFunName_; }
            set { strFunName_ = value; }
        }

        public List<string> ListStack
        {
            get { return ListStack_; }
            set { ListStack_ = value; }
        }

        public string strLogContent
        {
            get { return strLogContent_; }
            set { strLogContent_ = value; }
        }

        public void SetStackList(string strText)
        {
            ListStack_ = strText.Split(new string[] { "-" }, StringSplitOptions.RemoveEmptyEntries).ToList();
        }

        public string GetContentText()
        {
            return nRepeatCount == 0 ? strLogContent : strLogContent + " #" + nRepeatCount.ToString();
        }

        public string GetTipText()
        {
            string strText = string.Empty;
            strText += "Client:" + strClientName + "\r\n";
            strText += "FunName:" + strFunName + "\r\n";
            strText += "File:" + strFileName + " Line:" + nLine.ToString() + "\r\n";
            strText += "Stack:";
            ListStack_.ForEach(strStack => strText += strStack + "\r\n");

            return strText;
        }

       public static T Copy<T>(T RealObject)
        {
            using (Stream objectStream = new MemoryStream())
            {
                IFormatter formatter = new BinaryFormatter();
                formatter.Serialize(objectStream, RealObject);
                objectStream.Seek(0, SeekOrigin.Begin);
                return (T)formatter.Deserialize(objectStream);  
            }
        }
    }

    public class CFindLogContent
    {
        public string strLogContent { get; set; }

        public int nIndex { get; set; }
    }
}
