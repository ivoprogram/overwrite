namespace overwrite_ui
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
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.checkTest = new System.Windows.Forms.CheckBox();
            this.listBlock = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.listFiles = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.listData = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.listContent = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.listUnit = new System.Windows.Forms.ComboBox();
            this.textPath = new System.Windows.Forms.TextBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.buttonPath = new System.Windows.Forms.Button();
            this.labelProgress = new System.Windows.Forms.Label();
            this.buttonOverwrite = new System.Windows.Forms.Button();
            this.buttonClose = new System.Windows.Forms.Button();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(19, 239);
            this.richTextBox1.Margin = new System.Windows.Forms.Padding(4);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(385, 88);
            this.richTextBox1.TabIndex = 1;
            this.richTextBox1.Text = "";
            // 
            // checkTest
            // 
            this.checkTest.AutoSize = true;
            this.checkTest.Location = new System.Drawing.Point(306, 23);
            this.checkTest.Margin = new System.Windows.Forms.Padding(4);
            this.checkTest.Name = "checkTest";
            this.checkTest.Size = new System.Drawing.Size(55, 21);
            this.checkTest.TabIndex = 4;
            this.checkTest.Text = "Test";
            this.checkTest.UseVisualStyleBackColor = true;
            // 
            // listBlock
            // 
            this.listBlock.FormattingEnabled = true;
            this.listBlock.Items.AddRange(new object[] {
            "512",
            "1024",
            "2048",
            "4096",
            "8192"});
            this.listBlock.Location = new System.Drawing.Point(19, 149);
            this.listBlock.Margin = new System.Windows.Forms.Padding(4);
            this.listBlock.Name = "listBlock";
            this.listBlock.Size = new System.Drawing.Size(104, 24);
            this.listBlock.TabIndex = 8;
            this.listBlock.Text = "4096";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(131, 153);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 17);
            this.label1.TabIndex = 9;
            this.label1.Text = "Block size";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(131, 27);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(67, 17);
            this.label2.TabIndex = 11;
            this.label2.Text = "Metadata";
            // 
            // listFiles
            // 
            this.listFiles.FormattingEnabled = true;
            this.listFiles.Items.AddRange(new object[] {
            "0",
            "10",
            "100",
            "1000"});
            this.listFiles.Location = new System.Drawing.Point(19, 23);
            this.listFiles.Margin = new System.Windows.Forms.Padding(4);
            this.listFiles.Name = "listFiles";
            this.listFiles.Size = new System.Drawing.Size(104, 24);
            this.listFiles.TabIndex = 10;
            this.listFiles.Text = "10";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(131, 59);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 17);
            this.label3.TabIndex = 13;
            this.label3.Text = "Data";
            // 
            // listData
            // 
            this.listData.FormattingEnabled = true;
            this.listData.Items.AddRange(new object[] {
            "0",
            "10",
            "100",
            "1000",
            "All"});
            this.listData.Location = new System.Drawing.Point(19, 53);
            this.listData.Margin = new System.Windows.Forms.Padding(4);
            this.listData.Name = "listData";
            this.listData.Size = new System.Drawing.Size(104, 24);
            this.listData.TabIndex = 12;
            this.listData.Text = "10";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(131, 120);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(89, 17);
            this.label4.TabIndex = 16;
            this.label4.Text = "Data content";
            // 
            // listContent
            // 
            this.listContent.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listContent.FormattingEnabled = true;
            this.listContent.Items.AddRange(new object[] {
            "Zero",
            "One",
            "Random"});
            this.listContent.Location = new System.Drawing.Point(19, 117);
            this.listContent.Margin = new System.Windows.Forms.Padding(4);
            this.listContent.Name = "listContent";
            this.listContent.Size = new System.Drawing.Size(104, 24);
            this.listContent.TabIndex = 15;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(131, 89);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(65, 17);
            this.label5.TabIndex = 18;
            this.label5.Text = "Data unit";
            // 
            // listUnit
            // 
            this.listUnit.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listUnit.FormattingEnabled = true;
            this.listUnit.Items.AddRange(new object[] {
            "Megabytes",
            "Gigabytes"});
            this.listUnit.Location = new System.Drawing.Point(19, 85);
            this.listUnit.Margin = new System.Windows.Forms.Padding(4);
            this.listUnit.Name = "listUnit";
            this.listUnit.Size = new System.Drawing.Size(104, 24);
            this.listUnit.TabIndex = 17;
            // 
            // textPath
            // 
            this.textPath.Location = new System.Drawing.Point(19, 181);
            this.textPath.Margin = new System.Windows.Forms.Padding(4);
            this.textPath.Name = "textPath";
            this.textPath.Size = new System.Drawing.Size(385, 23);
            this.textPath.TabIndex = 19;
            this.textPath.Text = "C:\\";
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(19, 211);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(385, 21);
            this.progressBar1.TabIndex = 20;
            // 
            // buttonPath
            // 
            this.buttonPath.Location = new System.Drawing.Point(411, 181);
            this.buttonPath.Name = "buttonPath";
            this.buttonPath.Size = new System.Drawing.Size(28, 23);
            this.buttonPath.TabIndex = 21;
            this.buttonPath.Text = "...";
            this.buttonPath.UseVisualStyleBackColor = true;
            this.buttonPath.Click += new System.EventHandler(this.buttonPath_Click);
            // 
            // labelProgress
            // 
            this.labelProgress.AutoSize = true;
            this.labelProgress.Location = new System.Drawing.Point(409, 213);
            this.labelProgress.Name = "labelProgress";
            this.labelProgress.Size = new System.Drawing.Size(28, 17);
            this.labelProgress.TabIndex = 22;
            this.labelProgress.Text = "0%";
            // 
            // buttonOverwrite
            // 
            this.buttonOverwrite.Location = new System.Drawing.Point(306, 47);
            this.buttonOverwrite.Name = "buttonOverwrite";
            this.buttonOverwrite.Size = new System.Drawing.Size(98, 28);
            this.buttonOverwrite.TabIndex = 23;
            this.buttonOverwrite.Text = "Overwrite";
            this.buttonOverwrite.UseVisualStyleBackColor = true;
            this.buttonOverwrite.Click += new System.EventHandler(this.buttonOverwrite_Click);
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(306, 82);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(98, 28);
            this.buttonClose.TabIndex = 24;
            this.buttonClose.Text = "Close";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabel1.LinkBehavior = System.Windows.Forms.LinkBehavior.NeverUnderline;
            this.linkLabel1.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkLabel1.Location = new System.Drawing.Point(224, 331);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(180, 17);
            this.linkLabel1.TabIndex = 25;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "https://ivoprogram.github.io";
            this.linkLabel1.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(293, 160);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(111, 17);
            this.label6.TabIndex = 26;
            this.label6.Text = "Destination path";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(455, 362);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.linkLabel1);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.buttonOverwrite);
            this.Controls.Add(this.labelProgress);
            this.Controls.Add(this.buttonPath);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.textPath);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.listUnit);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.listContent);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.listData);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.listFiles);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.listBlock);
            this.Controls.Add(this.checkTest);
            this.Controls.Add(this.richTextBox1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Overwrite";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.CheckBox checkTest;
        private System.Windows.Forms.ComboBox listBlock;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox listFiles;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox listData;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox listContent;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox listUnit;
        private System.Windows.Forms.TextBox textPath;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button buttonPath;
        private System.Windows.Forms.Label labelProgress;
        private System.Windows.Forms.Button buttonOverwrite;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.Label label6;

    }
}

