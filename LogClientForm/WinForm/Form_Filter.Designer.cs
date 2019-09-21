namespace Log.WinForm
{
    partial class Form_Filter
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
            this.label1 = new System.Windows.Forms.Label();
            this.LnkLogContent = new System.Windows.Forms.LinkLabel();
            this.LnkClientName = new System.Windows.Forms.LinkLabel();
            this.label5 = new System.Windows.Forms.Label();
            this.LnkLogType = new System.Windows.Forms.LinkLabel();
            this.label6 = new System.Windows.Forms.Label();
            this.btnFilter = new System.Windows.Forms.Button();
            this.txtText = new System.Windows.Forms.TextBox();
            this.CCB = new System.Windows.Forms.ComboBox();
            this.btnHide = new System.Windows.Forms.Button();
            this.btnRestore = new System.Windows.Forms.Button();
            this.cbAbandon = new System.Windows.Forms.CheckBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "LogContent:";
            // 
            // LnkLogContent
            // 
            this.LnkLogContent.AutoSize = true;
            this.LnkLogContent.Location = new System.Drawing.Point(99, 15);
            this.LnkLogContent.Name = "LnkLogContent";
            this.LnkLogContent.Size = new System.Drawing.Size(35, 12);
            this.LnkLogContent.TabIndex = 2;
            this.LnkLogContent.TabStop = true;
            this.LnkLogContent.Text = "Empty";
            this.LnkLogContent.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LnkLogContent_LinkClicked);
            // 
            // LnkClientName
            // 
            this.LnkClientName.AutoSize = true;
            this.LnkClientName.Location = new System.Drawing.Point(348, 15);
            this.LnkClientName.Name = "LnkClientName";
            this.LnkClientName.Size = new System.Drawing.Size(35, 12);
            this.LnkClientName.TabIndex = 10;
            this.LnkClientName.TabStop = true;
            this.LnkClientName.Text = "Empty";
            this.LnkClientName.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LnkClientName_LinkClicked);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(271, 15);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(71, 12);
            this.label5.TabIndex = 9;
            this.label5.Text = "ClientName:";
            // 
            // LnkLogType
            // 
            this.LnkLogType.AutoSize = true;
            this.LnkLogType.Location = new System.Drawing.Point(99, 47);
            this.LnkLogType.Name = "LnkLogType";
            this.LnkLogType.Size = new System.Drawing.Size(35, 12);
            this.LnkLogType.TabIndex = 12;
            this.LnkLogType.TabStop = true;
            this.LnkLogType.Text = "Empty";
            this.LnkLogType.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LnkLogType_LinkClicked);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 47);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 12);
            this.label6.TabIndex = 11;
            this.label6.Text = "LogType:";
            // 
            // btnFilter
            // 
            this.btnFilter.Location = new System.Drawing.Point(139, 78);
            this.btnFilter.Name = "btnFilter";
            this.btnFilter.Size = new System.Drawing.Size(105, 46);
            this.btnFilter.TabIndex = 17;
            this.btnFilter.Text = "Filter";
            this.btnFilter.UseVisualStyleBackColor = true;
            this.btnFilter.Click += new System.EventHandler(this.btnFilter_Click);
            // 
            // txtText
            // 
            this.txtText.Location = new System.Drawing.Point(24, 78);
            this.txtText.Name = "txtText";
            this.txtText.Size = new System.Drawing.Size(100, 21);
            this.txtText.TabIndex = 18;
            this.txtText.Visible = false;
            this.txtText.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtText_KeyDown);
            // 
            // CCB
            // 
            this.CCB.FormattingEnabled = true;
            this.CCB.Location = new System.Drawing.Point(12, 108);
            this.CCB.Name = "CCB";
            this.CCB.Size = new System.Drawing.Size(121, 20);
            this.CCB.TabIndex = 19;
            this.CCB.Visible = false;
            this.CCB.SelectedIndexChanged += new System.EventHandler(this.CCB_SelectedIndexChanged);
            // 
            // btnHide
            // 
            this.btnHide.Location = new System.Drawing.Point(250, 78);
            this.btnHide.Name = "btnHide";
            this.btnHide.Size = new System.Drawing.Size(104, 46);
            this.btnHide.TabIndex = 20;
            this.btnHide.Text = "Hide";
            this.btnHide.UseVisualStyleBackColor = true;
            this.btnHide.Click += new System.EventHandler(this.btnHide_Click);
            // 
            // btnRestore
            // 
            this.btnRestore.Location = new System.Drawing.Point(363, 78);
            this.btnRestore.Name = "btnRestore";
            this.btnRestore.Size = new System.Drawing.Size(104, 46);
            this.btnRestore.TabIndex = 21;
            this.btnRestore.Text = "Restore";
            this.btnRestore.UseVisualStyleBackColor = true;
            this.btnRestore.Click += new System.EventHandler(this.btnRestore_Click);
            // 
            // cbAbandon
            // 
            this.cbAbandon.AutoSize = true;
            this.cbAbandon.Location = new System.Drawing.Point(401, 47);
            this.cbAbandon.Name = "cbAbandon";
            this.cbAbandon.Size = new System.Drawing.Size(66, 16);
            this.cbAbandon.TabIndex = 22;
            this.cbAbandon.Text = "Abandon";
            this.cbAbandon.UseVisualStyleBackColor = true;
            this.cbAbandon.CheckedChanged += new System.EventHandler(this.cbAbandon_CheckedChanged);
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(223, 47);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(48, 16);
            this.checkBox1.TabIndex = 23;
            this.checkBox1.Text = "Keep";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // checkBox2
            // 
            this.checkBox2.AutoSize = true;
            this.checkBox2.Location = new System.Drawing.Point(287, 46);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(108, 16);
            this.checkBox2.TabIndex = 24;
            this.checkBox2.Text = "SearchInResult";
            this.checkBox2.UseVisualStyleBackColor = true;
            // 
            // Form_Filter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(479, 140);
            this.Controls.Add(this.checkBox2);
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.cbAbandon);
            this.Controls.Add(this.btnRestore);
            this.Controls.Add(this.btnHide);
            this.Controls.Add(this.CCB);
            this.Controls.Add(this.txtText);
            this.Controls.Add(this.btnFilter);
            this.Controls.Add(this.LnkLogType);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.LnkClientName);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.LnkLogContent);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Form_Filter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Form_Filter";
            this.Load += new System.EventHandler(this.Form_Filter_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form_Filter_KeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.LinkLabel LnkLogContent;
        private System.Windows.Forms.LinkLabel LnkClientName;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.LinkLabel LnkLogType;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnFilter;
        private System.Windows.Forms.TextBox txtText;
        private System.Windows.Forms.ComboBox CCB;
        private System.Windows.Forms.Button btnHide;
        private System.Windows.Forms.Button btnRestore;
        private System.Windows.Forms.CheckBox cbAbandon;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.CheckBox checkBox2;
    }
}