using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Linq;

namespace Log.Code
{
    public class CXmlConfig
    {
        public bool InitConfig()
        {
            try
            {
                XElement XElement_ = null;
                if (!LoadXElement(ref XElement_))
                    return false;

                var Elements = from itm in XElement_.Elements("LogType") select itm;
                if (!GetLogTypeColor(Elements, "Debug", ref Code.CConfigValue.DebugColor))
                    return false;

                if (!GetLogTypeColor(Elements, "Warning", ref Code.CConfigValue.WarningColor))
                    return false;

                if (!GetLogTypeColor(Elements, "Exception", ref Code.CConfigValue.ExceptionColor))
                    return false;

                if (!GetLogTypeColor(Elements, "Custome", ref Code.CConfigValue.CustomeColor))
                    return false;

                if (!GetLogValue(XElement_.Elements(), "MaxLogCount", ref Code.CConfigValue.nMaxLogCount))
                    return false;

                if (!GetLogBool(XElement_.Elements(), "ConfirmMsgBox", ref Code.CConfigValue.bConfirmMsgBox))
                    return false;

                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in InitConfig:" + ex.Message);
            }
            return false;
        }

        public bool SaveConfig()
        {
            try
            {
                XElement XElement_ = null;
                if (!LoadXElement(ref XElement_))
                    return false;

                var Elements = from itm in XElement_.Elements("LogType") select itm;
                if (!SetLogTypeColor(Elements, "Debug", Code.CConfigValue.DebugColor))
                    return false;

                if (!SetLogTypeColor(Elements, "Warning", Code.CConfigValue.WarningColor))
                    return false;

                if (!SetLogTypeColor(Elements, "Exception", Code.CConfigValue.ExceptionColor))
                    return false;

                if (!SetLogTypeColor(Elements, "Custome", Code.CConfigValue.CustomeColor))
                    return false;

                if (!SetLogText(XElement_.Elements(), "MaxLogCount", Code.CConfigValue.nMaxLogCount.ToString()))
                    return false;

                Code.CConfigValue.bConfirmMsgBox = false;
                if (!SetLogText(XElement_.Elements(), "ConfirmMsgBox", Code.CConfigValue.bConfirmMsgBox == true ? "1" : "0"))
                    return false;

                if (!SaveXElement(XElement_))
                    return false;
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in SaveConfig:" + ex.Message);
            }
            return false;
        }

        private bool LoadXElement(ref XElement XElement_)
        {
            try
            {
                var strFilePath = System.Environment.CurrentDirectory + @"\Config.xml";
                if (!File.Exists(strFilePath))
                {
                    MessageBox.Show("Config UnExist:" + strFilePath);
                    return false;
                }

                XElement_ = XElement.Load(strFilePath);
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in GetXElement:" + ex.Message);
            }
            return false;
        }

        private bool SaveXElement(XElement XElement_)
        {
            try
            {
                var strFilePath = System.Environment.CurrentDirectory + @"\Config.xml";
                if (!File.Exists(strFilePath))
                {
                    MessageBox.Show("Config UnExist:" + strFilePath);
                    return false;
                }

                XElement_.Save(strFilePath);
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception in GetXElement:" + ex.Message);
            }
            return false;
        }

        private bool GetLogTypeColor(IEnumerable<XElement> Elements, string ConfigName, ref Color LogTypeColor)
        {
            foreach (var item in Elements.Elements().Attributes())
            {
                if (item.Value == ConfigName)
                {
                    LogTypeColor = ColorTranslator.FromHtml(item.Parent.Value);
                    return true;
                }
            }

            MessageBox.Show("UnExist " + ConfigName + " in LogType!");
            return false;
        }

        private bool SetLogTypeColor(IEnumerable<XElement> Elements, string ConfigName, Color LogTypeColor)
        {
            foreach (var item in Elements.Elements().Attributes())
            {
                if (item.Value == ConfigName)
                {
                    item.Parent.Value = ColorTranslator.ToHtml(LogTypeColor);
                    return true;
                }
            }

            MessageBox.Show("UnExist " + ConfigName + " in LogType!");
            return false;
        }

        private bool GetLogValue(IEnumerable<XElement> Elements, string ConfigName, ref int Value)
        {
            foreach (var item in Elements)
            {
                if (item.Name.LocalName == ConfigName)
                {
                    Value = int.Parse(item.Value);
                    return true;
                }
            }

            MessageBox.Show("UnExist " + ConfigName + " in LogType!");
            return false;
        }

        private bool SetLogText(IEnumerable<XElement> Elements, string ConfigName, string Value)
        {
            foreach (var item in Elements)
            {
                if (item.Name.LocalName == ConfigName)
                {
                    item.Value = Value;
                    return true;
                }
            }

            MessageBox.Show("UnExist " + ConfigName + " in LogType!");
            return false;
        }

        private bool GetLogBool(IEnumerable<XElement> Elements, string ConfigName, ref bool Value)
        {
            foreach (var item in Elements)
            {
                if (item.Name.LocalName == ConfigName)
                {
                    Value = item.Value == "1" ? true : false;
                    return true;
                }
            }

            MessageBox.Show("UnExist " + ConfigName + " in LogType!");
            return false;
        }
        public static T Clone<T>(T RealObject)
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
}
