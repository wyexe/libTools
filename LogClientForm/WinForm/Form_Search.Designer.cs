namespace Log.WinForm
{
    partial class Form_Search
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.txtKeyword = new System.Windows.Forms.TextBox();
            this.btnFind = new System.Windows.Forms.Button();
            this.LbxFindLog = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // txtKeyword
            // 
            this.txtKeyword.AcceptsReturn = true;
            this.txtKeyword.Location = new System.Drawing.Point(12, 21);
            this.txtKeyword.Name = "txtKeyword";
            this.txtKeyword.Size = new System.Drawing.Size(280, 21);
            this.txtKeyword.TabIndex = 0;
            this.txtKeyword.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtKeyword_KeyDown);
            // 
            // btnFind
            // 
            this.btnFind.Location = new System.Drawing.Point(12, 310);
            this.btnFind.Name = "btnFind";
            this.btnFind.Size = new System.Drawing.Size(280, 73);
            this.btnFind.TabIndex = 3;
            this.btnFind.Text = "Find";
            this.btnFind.UseVisualStyleBackColor = true;
            this.btnFind.Click += new System.EventHandler(this.btnFind_Click);
            // 
            // LbxFindLog
            // 
            this.LbxFindLog.FormattingEnabled = true;
            this.LbxFindLog.ItemHeight = 12;
            this.LbxFindLog.Location = new System.Drawing.Point(12, 48);
            this.LbxFindLog.Name = "LbxFindLog";
            this.LbxFindLog.Size = new System.Drawing.Size(280, 256);
            this.LbxFindLog.TabIndex = 4;
            this.LbxFindLog.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LbxFindLog_MouseDoubleClick);
            // 
            // Form_Search
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(313, 395);
            this.Controls.Add(this.LbxFindLog);
            this.Controls.Add(this.btnFind);
            this.Controls.Add(this.txtKeyword);
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form_Search";
            this.Opacity = 0.9D;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form_Search";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form_Search_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form_Search_FormClosed);
            this.Load += new System.EventHandler(this.Form_Search_Load);
            this.SizeChanged += new System.EventHandler(this.Form_Search_SizeChanged);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form_Search_KeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtKeyword;
        private System.Windows.Forms.Button btnFind;
        private System.Windows.Forms.ListBox LbxFindLog;
    }
}