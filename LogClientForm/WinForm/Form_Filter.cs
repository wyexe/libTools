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
    public enum em_Filter_Type
    {
        em_Filter_Type_LogContent = 0x2,
        em_Filter_Type_ClientName = 0x4,
        em_Filter_Type_LogType = 0x8,
    }

    public struct FilterContent
    {
        public em_Filter_Type emFilterType { get; set; }
        public object Value { get; set; }

        public string GetValueText()
        {
            return Value == null ? "" : Value.ToString();
        }
    }
    public struct ComboBoxItem
    {
        public string Text { get; set; }
        public object Value { get; set; }

        public override string ToString()
        {
            return Text.ToString();
        }
    }
    public partial class Form_Filter : Form
    {
        public Form_Filter()
        {
            InitializeComponent();
        }

        private void Form_Filter_Load(object sender, EventArgs e)
        {

        }

        private void ShowTextBox(LinkLabel Lnk)
        {
            if (txtText.Visible)
            {
                MessageBox.Show("TextBox Editing……");
                return;
            }
            Lnk.Visible = false;
            txtText.Location = Lnk.Location;
            txtText.Tag = Lnk;
            txtText.Text = Lnk.Text == "Empty" ? "" : Lnk.Text;
            txtText.Visible = true;
        }

        private void LnkLogContent_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            ShowTextBox(LnkLogContent);
        }

        private void txtText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                (txtText.Tag as LinkLabel).Text = txtText.Text.Trim() == "" ? "Empty" : txtText.Text;
                (txtText.Tag as LinkLabel).Visible = true;
                txtText.Text = "";
                txtText.Visible = false;
            }
        }

        private void ShowCombox(LinkLabel Lnk, List<ComboBoxItem> ItemList)
        {
            if (CCB.Visible)
            {
                MessageBox.Show("Combox Editing……");
                return;
            }
            Lnk.Visible = false;
            CCB.Location = Lnk.Location;
            CCB.Tag = Lnk;
            CCB.SelectedIndex = -1;
            CCB.Items.Clear();
            ItemList.ForEach((str) => { CCB.Items.Add(str); });
            CCB.SelectedIndex = 0;
            CCB.Visible = true;
        }

        private void LnkClientName_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            lock (Code.CConfigValue.ListClientName)
            {
                if (Code.CConfigValue.ListClientName.Count == 0)
                {
                    MessageBox.Show("UnExist ClientName!");
                    return;
                }

                var lst = new List<ComboBoxItem>() { new ComboBoxItem(){Text = "Empty", Value = "Empty"} };
                Code.CConfigValue.ListClientName.ForEach((str) => lst.Add(new ComboBoxItem() { Text = str, Value = str }));

                ShowCombox(LnkClientName, lst);
            }
            
        }

        private void CCB_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (CCB.SelectedIndex != -1)
            {
                var ComBoxItem = (ComboBoxItem)CCB.Items[CCB.SelectedIndex];
                (CCB.Tag as LinkLabel).Text = ComBoxItem.Text.ToString();
                (CCB.Tag as LinkLabel).Tag = ComBoxItem.Value;
                (CCB.Tag as LinkLabel).Visible = true;
                CCB.Visible = false;
            }
           
        }

        private void LnkLogType_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            var lst = new List<ComboBoxItem>() 
            { 
                new ComboBoxItem() { Text = "Empty", Value = Code.em_Log_Type.em_Log_Type_Invalid } ,
                new ComboBoxItem() { Text = "Debug", Value = Code.em_Log_Type.em_Log_Type_Debug },
                new ComboBoxItem() { Text = "Warning", Value = Code.em_Log_Type.em_Log_Type_Warning },
                new ComboBoxItem() { Text = "Custome", Value = Code.em_Log_Type.em_Log_Type_Custome },
                new ComboBoxItem() { Text = "Exception", Value = Code.em_Log_Type.em_Log_Type_Exception }
            };
            ShowCombox(LnkLogType, lst);
        }

        private void SetFilterContent_TextBox(LinkLabel Lnk, em_Filter_Type emType, ref List<FilterContent> ListFilterContent)
        {
            ListFilterContent.Add(new FilterContent() { emFilterType = emType, Value = Lnk.Text.Trim() });
        }

        private void SetFilterContent_Combox(LinkLabel Lnk, em_Filter_Type emType, ref List<FilterContent> ListFilterContent)
        {
            ListFilterContent.Add(new FilterContent() { emFilterType = emType, Value = Lnk.Tag });
        }

        private void btnFilter_Click(object sender, EventArgs e)
        {
            if (txtText.Visible)
            {
                (txtText.Tag as LinkLabel).Text = txtText.Text.Trim() == "" ? "Empty" : txtText.Text;
                (txtText.Tag as LinkLabel).Visible = true;
                txtText.Text = "";
                txtText.Visible = false;
            }

            List<FilterContent> ListFilterContent = new List<FilterContent>();
            SetFilterContent_TextBox(LnkLogContent, em_Filter_Type.em_Filter_Type_LogContent, ref ListFilterContent);
            SetFilterContent_TextBox(LnkClientName, em_Filter_Type.em_Filter_Type_ClientName, ref ListFilterContent);
            SetFilterContent_Combox(LnkLogType, em_Filter_Type.em_Filter_Type_LogType, ref ListFilterContent);

            Action<List<WinForm.FilterContent>, bool> fnFilterPtr = this.Tag as Action<List<WinForm.FilterContent>, bool>;
            fnFilterPtr(ListFilterContent, false);
            this.Hide();
        }

        private void Form_Filter_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape || (e.Control && e.KeyCode == Keys.L))
            {
                this.Hide();
            }
        }

        private void btnHide_Click(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void btnRestore_Click(object sender, EventArgs e)
        {
            Action<List<WinForm.FilterContent>, bool> fnFilterPtr = this.Tag as Action<List<WinForm.FilterContent>, bool>;
            fnFilterPtr(new List<FilterContent>(), true);
        }

        private void cbAbandon_CheckedChanged(object sender, EventArgs e)
        {
            Code.CConfigValue.bAbandon = cbAbandon.Checked;
        }
    }
}
