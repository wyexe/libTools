namespace Log
{
    partial class Form1
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.searchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.filterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.count0ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.warning0ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exception0ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dgvLog = new System.Windows.Forms.DataGridView();
            this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CCB_ExpressionText = new System.Windows.Forms.ComboBox();
            this.btnSumit = new System.Windows.Forms.Button();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvLog)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.optionToolStripMenuItem,
            this.count0ToolStripMenuItem,
            this.warning0ToolStripMenuItem,
            this.exception0ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(625, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.saveToolStripMenuItem.Text = "Save";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.searchToolStripMenuItem,
            this.clearToolStripMenuItem,
            this.filterToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // searchToolStripMenuItem
            // 
            this.searchToolStripMenuItem.Name = "searchToolStripMenuItem";
            this.searchToolStripMenuItem.Size = new System.Drawing.Size(109, 22);
            this.searchToolStripMenuItem.Text = "Search";
            this.searchToolStripMenuItem.Click += new System.EventHandler(this.searchToolStripMenuItem_Click);
            // 
            // clearToolStripMenuItem
            // 
            this.clearToolStripMenuItem.Name = "clearToolStripMenuItem";
            this.clearToolStripMenuItem.Size = new System.Drawing.Size(109, 22);
            this.clearToolStripMenuItem.Text = "Clear";
            this.clearToolStripMenuItem.Click += new System.EventHandler(this.clearToolStripMenuItem_Click);
            // 
            // filterToolStripMenuItem
            // 
            this.filterToolStripMenuItem.Name = "filterToolStripMenuItem";
            this.filterToolStripMenuItem.Size = new System.Drawing.Size(109, 22);
            this.filterToolStripMenuItem.Text = "Filter";
            this.filterToolStripMenuItem.Click += new System.EventHandler(this.filterToolStripMenuItem_Click);
            // 
            // optionToolStripMenuItem
            // 
            this.optionToolStripMenuItem.Name = "optionToolStripMenuItem";
            this.optionToolStripMenuItem.Size = new System.Drawing.Size(76, 20);
            this.optionToolStripMenuItem.Text = "√ AutoRoll";
            this.optionToolStripMenuItem.Click += new System.EventHandler(this.optionToolStripMenuItem_Click);
            // 
            // count0ToolStripMenuItem
            // 
            this.count0ToolStripMenuItem.Name = "count0ToolStripMenuItem";
            this.count0ToolStripMenuItem.Size = new System.Drawing.Size(64, 20);
            this.count0ToolStripMenuItem.Text = "Count: 0";
            this.count0ToolStripMenuItem.Click += new System.EventHandler(this.count0ToolStripMenuItem_Click);
            // 
            // warning0ToolStripMenuItem
            // 
            this.warning0ToolStripMenuItem.Name = "warning0ToolStripMenuItem";
            this.warning0ToolStripMenuItem.Size = new System.Drawing.Size(76, 20);
            this.warning0ToolStripMenuItem.Text = "Warning: 0";
            this.warning0ToolStripMenuItem.Click += new System.EventHandler(this.warning0ToolStripMenuItem_Click);
            // 
            // exception0ToolStripMenuItem
            // 
            this.exception0ToolStripMenuItem.Name = "exception0ToolStripMenuItem";
            this.exception0ToolStripMenuItem.Size = new System.Drawing.Size(82, 20);
            this.exception0ToolStripMenuItem.Text = "Exception: 0";
            this.exception0ToolStripMenuItem.Click += new System.EventHandler(this.exception0ToolStripMenuItem_Click);
            // 
            // dgvLog
            // 
            this.dgvLog.AllowDrop = true;
            this.dgvLog.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvLog.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column3,
            this.Column2,
            this.Column1});
            this.dgvLog.Location = new System.Drawing.Point(12, 27);
            this.dgvLog.Name = "dgvLog";
            this.dgvLog.RowTemplate.Height = 23;
            this.dgvLog.Size = new System.Drawing.Size(595, 502);
            this.dgvLog.TabIndex = 1;
            this.dgvLog.VirtualMode = true;
            this.dgvLog.CellValueNeeded += new System.Windows.Forms.DataGridViewCellValueEventHandler(this.dgvLog_CellValueNeeded);
            this.dgvLog.DragDrop += new System.Windows.Forms.DragEventHandler(this.dgvLog_DragDrop);
            this.dgvLog.DragEnter += new System.Windows.Forms.DragEventHandler(this.dgvLog_DragEnter);
            this.dgvLog.KeyDown += new System.Windows.Forms.KeyEventHandler(this.dgvLog_KeyDown);
            // 
            // Column3
            // 
            this.Column3.HeaderText = "Index";
            this.Column3.Name = "Column3";
            this.Column3.Width = 50;
            // 
            // Column2
            // 
            this.Column2.HeaderText = "Tick";
            this.Column2.Name = "Column2";
            // 
            // Column1
            // 
            this.Column1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Column1.HeaderText = "Text";
            this.Column1.Name = "Column1";
            // 
            // CCB_ExpressionText
            // 
            this.CCB_ExpressionText.FormattingEnabled = true;
            this.CCB_ExpressionText.Location = new System.Drawing.Point(12, 535);
            this.CCB_ExpressionText.Name = "CCB_ExpressionText";
            this.CCB_ExpressionText.Size = new System.Drawing.Size(514, 20);
            this.CCB_ExpressionText.TabIndex = 2;
            this.CCB_ExpressionText.KeyDown += new System.Windows.Forms.KeyEventHandler(this.CCB_ExpressionText_KeyDown);
            // 
            // btnSumit
            // 
            this.btnSumit.Location = new System.Drawing.Point(532, 532);
            this.btnSumit.Name = "btnSumit";
            this.btnSumit.Size = new System.Drawing.Size(75, 23);
            this.btnSumit.TabIndex = 3;
            this.btnSumit.Text = "Sumit";
            this.btnSumit.UseVisualStyleBackColor = true;
            this.btnSumit.Click += new System.EventHandler(this.btnSumit_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(625, 561);
            this.Controls.Add(this.btnSumit);
            this.Controls.Add(this.CCB_ExpressionText);
            this.Controls.Add(this.dgvLog);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "LogForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.SizeChanged += new System.EventHandler(this.Form1_SizeChanged);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvLog)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem searchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem clearToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem filterToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem optionToolStripMenuItem;
        private System.Windows.Forms.DataGridView dgvLog;
        private System.Windows.Forms.ComboBox CCB_ExpressionText;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column2;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
        private System.Windows.Forms.ToolStripMenuItem count0ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem warning0ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exception0ToolStripMenuItem;
        private System.Windows.Forms.Button btnSumit;
    }
}

