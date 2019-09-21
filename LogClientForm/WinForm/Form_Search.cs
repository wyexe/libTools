using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Log.WinForm
{
    public partial class Form_Search : Form
    {
        public Form_Search()
        {
            InitializeComponent();
        }

        private void txtKeyword_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape || (e.Control && e.KeyCode == Keys.F))
            {
                this.Hide();
            }
            else if (e.KeyCode == Keys.Return)
            {
                 btnFind_Click(sender, e);
            }
        }

        private void Form_Search_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape || (e.Control && e.KeyCode == Keys.F))
            {
                this.Hide();
            }
            else if (e.KeyCode == Keys.Enter)
            {
                btnFind_Click(sender, e);
            }
        }

        private void Form_Search_Load(object sender, EventArgs e)
        {

        }

        private void Form_Search_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private List<Code.CFindLogContent> ListLogResult = new List<Code.CFindLogContent>();
        private void btnFind_Click(object sender, EventArgs e)
        {
            try
            {
                Func<string, List<Code.CFindLogContent>> fnSearch = this.Tag as Func<string, List<Code.CFindLogContent>>;
                if (txtKeyword.Text.Trim() == "")
                    return;

                LbxFindLog.Items.Clear();
                ListLogResult = fnSearch(txtKeyword.Text);
                if (ListLogResult.Count == 0)
                    return;

                ListLogResult.ForEach(LogResult => LbxFindLog.Items.Add(LogResult.strLogContent));
            }
            catch (Exception ex)
            {
                MessageBox.Show("Search_Keydown:" + ex.Message);
            }
        }

        public Action<int> fnSelectRow = null;
        private void LbxFindLog_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            int nIndex = LbxFindLog.IndexFromPoint(e.Location);
            if (nIndex == -1)
                return;

            fnSelectRow(ListLogResult[nIndex].nIndex);
        }

        private void Form_Search_FormClosed(object sender, FormClosedEventArgs e)
        {
            
        }

        private void Form_Search_SizeChanged(object sender, EventArgs e)
        {
            LbxFindLog.Width = this.Width - 40;
            LbxFindLog.Height = this.Height - 170;
            btnFind.Width = this.Width - 40;
            btnFind.Location = new Point(12, this.Height - 124);
            txtKeyword.Width = this.Width - 40; 
        }
    }
}
