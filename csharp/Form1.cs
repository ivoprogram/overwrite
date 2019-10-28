
/*
TITLE:			Overwrite
DESCRIPTION:	Overwrite empty space on disk, write small files and data.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
DATE:			2019-10-28
VERSION:		1.2

*/


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.Diagnostics;


namespace overwrite_ui
{
    public partial class Form1 : Form
    {
        // 
        byte[] block;           // Block data
        int blockSize = 4096;   // Block size
        int files = 0;          // Number of files
        int filesCont = 0;      // Counter for write and clean
        int data = 0;           // Quantity of data
        string filePrefix = "000000000-";   // File prefix

        // 
        public Form1()
        {
            InitializeComponent();
            Initialize();
        }

        // Initialize
        private void Initialize()
        {
            // 
            textPath.Text = Path.GetTempPath();
            listUnit.SelectedIndex = 0;
            listContent.SelectedIndex = 0;
        }

        //
        private void buttonOverwrite_Click(object sender, EventArgs e)
        {
            Start();
        }// 

        // Start
        private void Start()
        {
            try
            {
                // Start
                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();
                richTextBox1.Clear();

                // Prepare, write, clean
                Prepare();
                WriteFiles();
                WriteData();
                if (!checkTest.Checked)
                {
                    CleanData();
                }

                // Done
                stopwatch.Stop();
                WriteTime(stopwatch);
                richTextBox1.AppendText("Done. \n");
            }
            catch (Exception exc)
            {
                richTextBox1.AppendText(exc.Message);
            }


        }

        // Prepare
        private void Prepare()
        {
            // Initialize parameters

            blockSize = Convert.ToInt32(listBlock.Text);
            block = new byte[blockSize];

            if (listFiles.Text == "All") { files = Int32.MaxValue; }
            else { files = Convert.ToInt32(listFiles.Text); }

            if (listData.Text == "All") { data = Int32.MaxValue; }
            else { data = Convert.ToInt32(listData.Text); }

            //
            if (!textPath.Text.EndsWith("\\") && !textPath.Text.EndsWith("/"))
            {
                textPath.Text += "\\";
            }


            // Set data content

            Random rand = new Random(DateTime.Now.Millisecond);

            if (listContent.Text == "Zero")
            {
                for (int index = 0; index < blockSize; index++)
                {
                    block[index] = 0;
                }
            }
            else if (listContent.Text == "One")
            {
                for (int index = 0; index < blockSize; index++)
                {
                    block[index] = 255;
                }
            }
            else // if (listContent.Text == "Random")
            {
                for (int index = 0; index < blockSize; index++)
                {
                    block[index] = (byte)rand.Next(1, 256);
                }
            }

        }


        // WriteFiles
        private void WriteFiles()
        {
            int cont = 1000;
            float stat = 0.1F;

            if (files < 1) { return; }

            // Status
            labelStatus.Text = "0%";
            progressBar1.Value = 0;
            richTextBox1.AppendText("Writing files \n");

            try
            {
                // Write files
                for (filesCont = 1; filesCont <= files; filesCont++)
                {
                    // Write
                    File.WriteAllBytes(
                        string.Format("{0}{1}{2}", textPath.Text, filePrefix, filesCont),
                        block);

                    // Statistic
                    if (files != Int32.MaxValue && filesCont > files * stat)
                    {
                        labelStatus.Text = string.Format("{0:00}%", stat * 100);
                        progressBar1.Value = Convert.ToInt32(stat * 100);
                        stat += 0.1F;
                    }

                    if (files == Int32.MaxValue && filesCont > cont)
                    {
                        labelStatus.Text = string.Format("{0}Th", cont);
                        progressBar1.Value = cont;
                        cont += cont;
                    }

                }// for

                labelStatus.Text = "100%";
                progressBar1.Value = 100;
            }
            catch (Exception exc)
            {
                if (filesCont < 2) { throw; }
            }

        }

        // WriteData
        private void WriteData()
        {
            int cont = 0;
            int blocks = 0;
            int blocksmb = 0;
            int blocksgb = 0;
            int blockscont = 0;
            float stat = 0.1F;

            if (data < 1) { return; }

            // Status
            labelStatus.Text = "0%";
            progressBar1.Value = 0;
            richTextBox1.AppendText("Writing data \n");

            // Blocks count
            blocksmb = (1024 * 1000) / blockSize;
            blocksgb = (1024 * 1000 * 1000) / blockSize;
            blockscont = blocksgb;

            if (listUnit.Text == "Megabytes") { blocks = data * blocksmb; }
            if (listUnit.Text == "Gigabytes") { blocks = data * blocksgb; }

            // Path
            string path = string.Format("{0}{1}{2}", textPath.Text, filePrefix, "0");

            // Write data
            using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
            {
                //
                for (int index = 1; index <= blocks; index++)
                {
                    // Write
                    writer.Write(block, 0, blockSize);

                    // Statistic
                    if (blocks != Int32.MaxValue && index > blocks * stat)
                    {
                        labelStatus.Text = string.Format("{0:00}%", stat * 100);
                        progressBar1.Value = Convert.ToInt32(stat * 100);
                        stat += 0.1F;
                    }

                    if (blocks == Int32.MaxValue && index > blockscont && cont < 10)
                    {
                        cont++;
                        labelStatus.Text = string.Format("{0}Gb", cont);
                        progressBar1.Value = cont;
                        blockscont += blocksgb;
                    }

                }// for

            }// using

            // Status
            labelStatus.Text = "100%";
            progressBar1.Value = 100;

        }

        // CleanData
        private void CleanData()
        {
            // 
            int index = 0;
            float stat = 0.1F;
            labelStatus.Text = "0%";
            progressBar1.Value = 0;
            richTextBox1.AppendText("Cleaning \n");

            // Clean data
            string path = string.Format("{0}{1}{2}", textPath.Text, filePrefix, "0");
            File.Delete(path);

            // Clean files
            for (index = 1; index <= filesCont && files > 0; index++)
            {
                File.Delete(string.Format("{0}{1}{2}", 
                    textPath.Text, filePrefix, index));

                // Statistic
                if (filesCont > files * stat)
                {
                    labelStatus.Text = string.Format("{0:00}%", stat * 100);
                    progressBar1.Value = Convert.ToInt32(stat * 100);
                    stat += 0.1F;
                }

            }// for

            // Status
            labelStatus.Text = "100%";
            progressBar1.Value = 100;

        }

        // WriteTime
        private void WriteTime(Stopwatch stopwatch)
        {
            string time = string.Format(
                "Time: {0}:{1}:{2}.{3} \n",
                stopwatch.Elapsed.Hours,
                stopwatch.Elapsed.Minutes,
                stopwatch.Elapsed.Seconds,
                stopwatch.Elapsed.Milliseconds
                );

            richTextBox1.AppendText(time);

        }

        private void buttonPath_Click(object sender, EventArgs e)
        {
            using (var dialog = new System.Windows.Forms.FolderBrowserDialog())
            {

                System.Windows.Forms.DialogResult result = dialog.ShowDialog();

                if (result == System.Windows.Forms.DialogResult.OK)
                {
                    textPath.Text = dialog.SelectedPath;
                }

            }

        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            //Application.Exit();
            this.Close();
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://ivoprogram.github.io");
        }


    }// class
}//


