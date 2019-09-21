using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Log.Code
{
    public enum em_Dgv_Clear
    {
        em_Dgv_Clear_ALL,
        em_Dgv_Clear_Half
    }
    public class CDgvServices
    {
        private static CDgvServices dgvServices_ = new CDgvServices();
        private static DataGridView dgv_ = null;
        private static Action<int> RemoveLogListFunPtr;
        public static CDgvServices GetInstance()
        {
            return dgvServices_;
        }

        public void SetDgv(DataGridView dgv)
        {
            dgv_ = dgv;
        }

        public void SetRemoveLogListFunPtr(Action<int> FunPtr)
        {
            RemoveLogListFunPtr = FunPtr;
        }

        public void Clear(em_Dgv_Clear emDgvClear)
        {
            if (emDgvClear == em_Dgv_Clear.em_Dgv_Clear_ALL)
            {
                if (Code.CConfigValue.bConfirmMsgBox && MessageBox.Show("Ready to Clear, Confirm?", "Confirm", MessageBoxButtons.YesNo) == DialogResult.No)
                    return;

                Code.CConfigValue.nExceptionCount = 0;
                Code.CConfigValue.nLogCount = 0;
                Code.CConfigValue.nWarningCount = 0;
                dgv_.Rows.Clear();
                RemoveLogListFunPtr(0);
            }
            else if (emDgvClear == em_Dgv_Clear.em_Dgv_Clear_Half)
            {
                int nCount = dgv_.Rows.Count / 2;
                Code.CConfigValue.nLogCount = nCount;
                RemoveLogListFunPtr(nCount);
                while (dgv_.Rows.Count > nCount)
                    dgv_.Rows.RemoveAt(0);
            }
            else
            {
                MessageBox.Show("Invalid emDgvClear!");
            }
        }

      
    }
}
