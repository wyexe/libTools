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
    public partial class Form_Load : Form
    {
        public Form_Load()
        {
            InitializeComponent();
        }

        private void Form_Load_Load(object sender, EventArgs e)
        {
            progressBar1.Maximum = 100;
            progressBar1.Step = 1;
            progressBar1.Value = 0;

            Worker();
        }

        private async void Worker()
        {
            int nTick = System.Environment.TickCount;
            do
            {
                if (progressBar1.Value != Code.CConfigValue.nProgressValue)
                    progressBar1.Value = Code.CConfigValue.nProgressValue;

                await Task.Run(() => { System.Threading.Thread.Sleep(100); });
            } while (progressBar1.Value != 100);
            this.Close();
        }
    }
}
