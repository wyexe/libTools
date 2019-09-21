using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Log
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
           
        }

        private bool m_bRecvLog = true;
        private Func<string, bool> fncFilterClientNamePtr = null;
        private Func<string, bool> fncFilterLogContentPtr = null;
        private Func<Code.em_Log_Type, bool> fncFilterLogTypePtr = null;
        private async void GetDLLMessageThread()
        {
            var sbLogText = new StringBuilder(1024* 10);
            //var strLastText = string.Empty;
            while (m_bRecvLog)
            {
                bool bAddText = false;
                List<String> ListLogText = new List<string>();
                int nEffectiveCount = 0;

                try
                {
                    lock (_Queue)
                    {
                        while (ListLogText.Count < 100 && _Queue.Count != 0)
                        {
                            ListLogText.Add(_Queue.Dequeue());
                        }
                    }

                    if (ListLogText.Count == 0)
                    {
                        await Task.Factory.StartNew(() => { Thread.Sleep(1000); });
                        continue;
                    }

                    for (int i = 0; i < ListLogText.Count; i++)
                    {
                        var LogTextArray = ListLogText[i].Split(new string[] { "\r\n" }, StringSplitOptions.None).ToList();
                        if (LogTextArray.Count < 6)
                            return;

                        var LogContent = new Log.Code.CLogContent();
                        LogContent.emLogType = (Code.em_Log_Type)int.Parse(LogTextArray[0]);
                        LogContent.strFunName = LogTextArray[1];
                        LogContent.strFileName = LogTextArray[2];
                        LogContent.nLine = int.Parse(LogTextArray[3]);
                        LogContent.strLogContent = LogTextArray[4];
                        LogContent.strTime = DateTime.Now.ToString("HH:mm:ss ms");
                        LogContent.strClientName = LogTextArray[5];

                        lock (Code.CConfigValue.ListDgvLog)
                        {
                            bool bFilter = true;
                            if (fncFilterClientNamePtr != null && !fncFilterClientNamePtr(LogContent.strClientName))
                                bFilter = false;
                            else if (fncFilterLogContentPtr != null && !fncFilterLogContentPtr(LogContent.strLogContent))
                                bFilter = false;
                            else if (fncFilterLogTypePtr != null && !fncFilterLogTypePtr(LogContent.emLogType))
                                bFilter = false;

                            if (Code.CConfigValue.bAbandon && !bFilter)
                                return;

                            switch (LogContent.emLogType)
                            {
                                case Log.Code.em_Log_Type.em_Log_Type_Debug:
                                    Code.CConfigValue.nLogCount += 1;
                                    break;
                                case Log.Code.em_Log_Type.em_Log_Type_Warning:
                                    Code.CConfigValue.nWarningCount += 1;
                                    break;
                                case Log.Code.em_Log_Type.em_Log_Type_Exception:
                                    Code.CConfigValue.nExceptionCount += 1;
                                    break;
                                default:
                                    break;
                            }

                            if (bFilter)
                            {
                                Code.CConfigValue.ListDgvLog.Add(LogContent);
                                nEffectiveCount += 1;
                                bAddText = true;
                            }

                            lock (Code.CConfigValue.ListClientLog)
                            {
                                Code.CConfigValue.ListClientLog.Add(LogContent);
                                Code.CConfigValue.AddClientName(LogContent.strClientName);
                            };
                        }
                    }
                   
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Excpetion in GetDLLMessageThread:" + ex.Message);
                    Application.Exit();
                }

                if (Code.CConfigValue.ListClientLog.Count >= Code.CConfigValue.nMaxLogCount)
                {
                    Code.CDgvServices.GetInstance().Clear(Code.em_Dgv_Clear.em_Dgv_Clear_Half);
                }

                if (!bAddText)
                    continue;

                for (int i = 0; i < nEffectiveCount; ++i)
                    dgvLog.Rows.Add();

                dgvLog.FirstDisplayedScrollingRowIndex = Code.CConfigValue.bDgvRoll ? dgvLog.Rows.Count - 1 : dgvLog.FirstDisplayedScrollingRowIndex;
                dgvLog.Update();
                count0ToolStripMenuItem.Text = "Count: " + Code.CConfigValue.nLogCount.ToString();
                warning0ToolStripMenuItem.Text = "Warning: " + Code.CConfigValue.nWarningCount.ToString();
                exception0ToolStripMenuItem.Text = "Exception: " + Code.CConfigValue.nExceptionCount.ToString();
            }
        }

        private void dgvLog_CellValueNeeded(object sender, DataGridViewCellValueEventArgs e)
        {
            if (e.RowIndex >= Code.CConfigValue.nMaxLogCount)
                Code.CDgvServices.GetInstance().Clear(Code.em_Dgv_Clear.em_Dgv_Clear_Half);

            if (e.RowIndex >= Code.CConfigValue.nMaxLogCount || e.RowIndex >= Code.CConfigValue.ListDgvLog.Count)
                return;

            lock (Code.CConfigValue.ListDgvLog)
            {
                if (e.ColumnIndex == 1)
                    e.Value = Code.CConfigValue.ListDgvLog[e.RowIndex].strTime;
                else if (e.ColumnIndex == 2)
                    e.Value = Code.CConfigValue.ListDgvLog[e.RowIndex].GetContentText();
                else if (e.ColumnIndex == 0)
                {
                    e.Value = e.RowIndex.ToString();
                    dgvLog.Rows[e.RowIndex].Cells[0].ToolTipText = Code.CConfigValue.ListDgvLog[e.RowIndex].GetTipText();
                }

                var DataGridViewCellStyle_ = new DataGridViewCellStyle();
                switch (Code.CConfigValue.ListDgvLog[e.RowIndex].emLogType)
                {
                    case Log.Code.em_Log_Type.em_Log_Type_Debug:
                        DataGridViewCellStyle_.ForeColor = Code.CConfigValue.DebugColor;
                        break;
                    case Log.Code.em_Log_Type.em_Log_Type_Warning:
                        DataGridViewCellStyle_.ForeColor = Code.CConfigValue.WarningColor;
                        break;
                    case Log.Code.em_Log_Type.em_Log_Type_Custome:
                        DataGridViewCellStyle_.ForeColor = Code.CConfigValue.CustomeColor;
                        break;
                    case Log.Code.em_Log_Type.em_Log_Type_Exception:
                        DataGridViewCellStyle_.ForeColor = Code.CConfigValue.ExceptionColor;
                        break;
                    default:
                        break;
                }
                for (int i = 0; i < dgvLog.Columns.Count; i++)
                {
                    dgvLog.Rows[e.RowIndex].Cells[i].Style = DataGridViewCellStyle_;
                }
            }
        }

        public const int WM_COPYDATA = 0x4A;
        public Queue<String> _Queue = new Queue<string>();
        public struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int cbData;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string lpData;
        }

        [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_COPYDATA)
            {
                COPYDATASTRUCT CopyStruct = new COPYDATASTRUCT();
                CopyStruct = (COPYDATASTRUCT)((Message)m).GetLParam(CopyStruct.GetType());
                if (CopyStruct.dwData.ToInt32() == 0x4C6F67)
                {
                    Task.Factory.StartNew(() => { lock (_Queue) { _Queue.Enqueue(CopyStruct.lpData); } });
                }
            }
           
            base.WndProc(ref m);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (!new Code.CXmlConfig().InitConfig())
                Application.Exit();


            dgvLog.AutoGenerateColumns = false;
            dgvLog.AllowUserToAddRows = false;
            dgvLog.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.EnableResizing;
            dgvLog.Columns.Cast<DataGridViewColumn>().ToList().ForEach(f => f.SortMode = DataGridViewColumnSortMode.NotSortable);

            Code.CDgvServices.GetInstance().SetDgv(dgvLog);

            // Set Remove List Function
            Code.CDgvServices.GetInstance().SetRemoveLogListFunPtr((int nCount) => 
            {
                lock (Code.CConfigValue.ListDgvLog)
                {
                    if (nCount == 0)
                        Code.CConfigValue.ListDgvLog.Clear();
                    else
                        Code.CConfigValue.ListDgvLog.RemoveRange(0, nCount);
                }
                lock (Code.CConfigValue.ListClientLog)
                {
                    if (nCount == 0)
                        Code.CConfigValue.ListClientLog.Clear();
                    else
                        Code.CConfigValue.ListClientLog.RemoveRange(0, nCount);
                }
            });

            // Set Search Text Function
            fnSearchTextPtr = (string strKeyWork) =>
            {
                var LogResult = new List<Code.CFindLogContent>();
                try
                {
                    lock (dgvLog)
                    {
                        for (int i = 0; i < dgvLog.Rows.Count; i++)
                        {
                            if (dgvLog.Rows[i].Cells[2].Value.ToString().IndexOf(strKeyWork) != -1)
                            {
                                LogResult.Add(new Code.CFindLogContent() { strLogContent = dgvLog.Rows[i].Cells[2].Value.ToString(), nIndex = i });
                                dgvLog.Rows[i].Selected = true;
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("fnSearchTextPtr:" + ex.Message);
                }
               return LogResult;
            };

            fnSelectRow = (int nIndex) =>
            {
                if (dgvLog.Rows.Count > nIndex)
                {
                    dgvLog.FirstDisplayedScrollingRowIndex = nIndex;
                    dgvLog.Rows[nIndex].Selected = true;
                }
                
            };

            // Set Filter Function
            fnFilterTextPtr = (List<WinForm.FilterContent> ListFilterContent, bool bRestore) =>
            {
                if (bRestore)
                {
                    fncFilterLogTypePtr = null;
                    fncFilterClientNamePtr = null;
                    fncFilterLogContentPtr = null;
                    lock (Code.CConfigValue.ListDgvLog)
                    {
                        lock (Code.CConfigValue.ListClientLog)
                        {
                            Code.CConfigValue.ListDgvLog = Code.CLogContent.Copy(Code.CConfigValue.ListClientLog);
                        }
                    }
                    RefushDgvLog();
                }
                ListFilterContent.ForEach((FilterContent_) =>
                {
                    switch (FilterContent_.emFilterType)
                    {
                        case Log.WinForm.em_Filter_Type.em_Filter_Type_LogContent:
                            FilterLogContent(FilterContent_.GetValueText());
                            break;
                        case Log.WinForm.em_Filter_Type.em_Filter_Type_ClientName:
                            FilterClientName(FilterContent_.GetValueText());
                            break;
                        case Log.WinForm.em_Filter_Type.em_Filter_Type_LogType:
                            FilterLogType(FilterContent_.Value == null ? Code.em_Log_Type.em_Log_Type_Invalid : (Code.em_Log_Type)FilterContent_.Value);
                            break;
                        default:
                            break;
                    }
                });
            };

            dgvLog.RowCount = 0;

            GetDLLMessageThread();
        }

        private void Form1_SizeChanged(object sender, EventArgs e)
        {
            dgvLog.Width = this.Width - 40;
            dgvLog.Height = this.Height - 50 - 20 - 40;
            CCB_ExpressionText.Width = this.Width - 120;
            btnSumit.Location = new Point(this.Width - 100, this.Height - 70);
            CCB_ExpressionText.Location = new Point(CCB_ExpressionText.Location.X, this.Height - 50 - 20);
        }

        private void SetautoRollToolStripMenuItemText()
        {
            if (optionToolStripMenuItem.Text == "AutoRoll")
            {
                optionToolStripMenuItem.Text = "√ AutoRoll";
                Code.CConfigValue.bDgvRoll = true;
            }
            else
            {
                optionToolStripMenuItem.Text = "AutoRoll";
                Code.CConfigValue.bDgvRoll = false;
            }
        }

        private Func<string, List<Code.CFindLogContent>> fnSearchTextPtr = null;
        private Action<int> fnSelectRow = null;
        private WinForm.Form_Search FormSearch = null;

        private Action<List<WinForm.FilterContent>, bool> fnFilterTextPtr = null;
        private WinForm.Form_Filter FormFilter = null;

        private void dgvLog_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.A)
            {
                SetautoRollToolStripMenuItemText();
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.X)
            {
                Clear();
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.F)
            {
                // Search
                e.Handled = true;
                searchToolStripMenuItem_Click(sender, e);
                
            }
            else if (e.Control && e.KeyCode == Keys.L)
            {
                // Filter
                e.Handled = true;
                filterToolStripMenuItem_Click(sender, e);
            }
            else if (e.Control && e.KeyCode == Keys.S)
            {
                // Save
                var strPath = new Code.CLogFile().SaveFile(Code.CConfigValue.ListDgvLog);
                MessageBox.Show("保存成功,路径:" + strPath);
            }
            else if (e.Control && e.KeyCode == Keys.O)
            {
                // Open
            }
        }

        private void optionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetautoRollToolStripMenuItemText();
        }

        private void Clear()
        {
            Code.CConfigValue.nLogCount = 0;
            Code.CConfigValue.nExceptionCount = 0;
            Code.CConfigValue.nWarningCount = 0;
            Code.CDgvServices.GetInstance().Clear(Code.em_Dgv_Clear.em_Dgv_Clear_ALL);
        }

        private void count0ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Clear?","Warning!",MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
            {
                Clear();
            }
        }

        private void warning0ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Clear?", "Warning!", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
            {
                Clear();
            }
        }

        private void exception0ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Clear?", "Warning!", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
            {
                Clear();
            }
        }

        private void RefushDgvLog()
        {
            dgvLog.Rows.Clear();
            dgvLog.RowCount = Code.CConfigValue.ListDgvLog.Count;
        }

        private void FilterClientName(string strClientName)
        {
            if (strClientName == "Empty")
                return;
            lock (Code.CConfigValue.ListDgvLog)
            {
                for (int i = 0; i < Code.CConfigValue.ListDgvLog.Count; i++)
                {
                    if (Code.CConfigValue.ListDgvLog[i].strClientName != strClientName)
                        Code.CConfigValue.ListDgvLog.RemoveAt(i--);
                }
            }
            fncFilterClientNamePtr = (string str) => str == strClientName;
            RefushDgvLog();
        }

        private void FilterLogContent(string strLogContent)
        {
            if (strLogContent == "Empty" || strLogContent.Trim() == "")
                return;
            lock (Code.CConfigValue.ListDgvLog)
            {
                for (int i = 0; i < Code.CConfigValue.ListDgvLog.Count; i++)
                {
                    if (Code.CConfigValue.ListDgvLog[i].strLogContent.IndexOf(strLogContent) == -1)
                        Code.CConfigValue.ListDgvLog.RemoveAt(i--);
                }
            }
            RefushDgvLog();
            fncFilterLogContentPtr = (string str) => str.IndexOf(strLogContent) != -1;
        }

        private void FilterLogType(Code.em_Log_Type emLogType)
        {
            if (emLogType == Code.em_Log_Type.em_Log_Type_Invalid)
                return;

            lock (Code.CConfigValue.ListDgvLog)
            {
                for (int i = 0; i < Code.CConfigValue.ListDgvLog.Count; i++)
                {
                    if (Code.CConfigValue.ListDgvLog[i].emLogType != emLogType)
                        Code.CConfigValue.ListDgvLog.RemoveAt(i--);
                }
            }
            RefushDgvLog();
            fncFilterLogTypePtr = (Code.em_Log_Type emType) => emType == emLogType;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_bRecvLog = false;
            Code.API.Release();
            Thread.Sleep(1000);
            System.Environment.Exit(0);
        }

        private void RunExpr(String Text)
        {
            if (Text.ToLower() == "clearcmd")
            {
                CCB_ExpressionText.Items.Clear();
                CCB_ExpressionText.Text = "";
                return;
            }

            // remove '-'
            if (Text.IndexOf("-") == 0)
                Text = Text.Substring(1);

            if (Text.IndexOf(" ") == -1)
                return;

            var strClientName = Text.Substring(0, Text.IndexOf(" "));
            Text = Text.Substring(Text.IndexOf(" ") + 1);

            var sbErrText = new StringBuilder(1024);
            var nRetCode = Code.API.WriteMsgToClient(strClientName, Text, sbErrText);
            if (nRetCode != 1)
            {
                MessageBox.Show(sbErrText.ToString());
                return;
            }

            if (!CCB_ExpressionText.Items.Contains(CCB_ExpressionText.Text))
                CCB_ExpressionText.Items.Add(CCB_ExpressionText.Text);
        }

        private void CCB_ExpressionText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode != Keys.Enter)
                return;

            RunExpr(CCB_ExpressionText.Text.Trim());
        }

        private void searchToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (FormSearch == null)
            {
                FormSearch = new WinForm.Form_Search();
                FormSearch.Owner = this;
                FormSearch.Tag = fnSearchTextPtr;
                FormSearch.fnSelectRow = fnSelectRow;
            }

            if (!FormSearch.Visible)
                FormSearch.Show();
            else
                FormSearch.Hide();
        }

        private void filterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (FormFilter == null)
            {
                FormFilter = new WinForm.Form_Filter();
                FormFilter.Owner = this;
                FormFilter.Tag = fnFilterTextPtr;
            }

            if (!FormFilter.Visible)
                FormFilter.Show();
            else
                FormFilter.Hide();
        }

        private async void LoadLogFile(string strFilePath)
        {
            var Lst = await new Code.CLogFile().GetLogFile(strFilePath);
            lock (dgvLog)
            {
                Code.CConfigValue.ListClientLog = Lst;
                Code.CConfigValue.ListDgvLog = Code.CLogContent.Copy(Code.CConfigValue.ListClientLog);
                dgvLog.RowCount = Code.CConfigValue.ListClientLog.Count;
            }
           
            count0ToolStripMenuItem.Text = "Count: " + Code.CConfigValue.nLogCount.ToString();
            warning0ToolStripMenuItem.Text = "Warning: " + Code.CConfigValue.nWarningCount.ToString();
            exception0ToolStripMenuItem.Text = "Exception: " + Code.CConfigValue.nExceptionCount.ToString();
        }

        private void dgvLog_DragDrop(object sender, DragEventArgs e)
        {
            Code.CConfigValue.nProgressValue = 0;
            LoadLogFile(((System.Array)e.Data.GetData(DataFormats.FileDrop)).GetValue(0).ToString());
            new WinForm.Form_Load().ShowDialog();
        }

        private void dgvLog_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Link : DragDropEffects.None;
        }

        private void clearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Code.CDgvServices.GetInstance().Clear(Code.em_Dgv_Clear.em_Dgv_Clear_ALL);
        }

        private void btnSumit_Click(object sender, EventArgs e)
        {
            RunExpr(CCB_ExpressionText.Text.Trim());
        }
    }
}
