
/*
TITLE:			Overwrite
DESCRIPTION:	Overwrite empty space on disk, metadata and data.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
DATE:			2019-10-30
VERSION:		1.3

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
        int bsize = 4096;       // Block size
        int dirs = 0;           // Number of dirs
        int dircont = 0;        // Counter for write and clean
        int data = 0;           // Quantity of data
        string prefix = "0";    // File prefix

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
            listUnit.SelectedIndex = 0;
            listContent.SelectedIndex = 0;
            //textPath.Text = Path.GetTempPath();

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

                WriteDirs();
                if (!checkTest.Checked) { CleanDirs(); }

                WriteData();
                if (!checkTest.Checked) { CleanData(); }

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

            bsize = Convert.ToInt32(listBlock.Text);
            block = new byte[bsize];
            dirs = Convert.ToInt32(listFiles.Text);

            if (listData.Text == "All") { data = Int32.MaxValue; }
            else { data = Convert.ToInt32(listData.Text); }

            //
            if (!textPath.Text.EndsWith("\\") && !textPath.Text.EndsWith("/"))
            {
                textPath.Text += "\\";
            }


            // Set data content
            if (listContent.Text == "Zero")
            {
                for (int index = 0; index < bsize; index++)
                {
                    block[index] = 0;
                }
            }
            else if (listContent.Text == "One")
            {
                for (int index = 0; index < bsize; index++)
                {
                    block[index] = 255;
                }
            }
            else // if (listContent.Text == "Random")
            {
                Random rand = new Random(DateTime.Now.Millisecond);
                for (int index = 0; index < bsize; index++)
                {
                    block[index] = (byte)rand.Next(1, 256);
                }
            }

        }

        // WriteDirs
        private void WriteDirs()
        {
            float stat = 0.1F;

            if (dirs == 0) { return; }

            try
            {
                // Status
                richTextBox1.AppendText("Writing metadata \n");
                Progress(0);

                // Write dirs
                for (dircont = 1; dircont <= dirs; dircont++)
                {
                    // Write
                    Directory.CreateDirectory(
                        string.Format("{0}{1}{2}", textPath.Text, prefix, dircont)
                        );

                    // Statistic
                    if (dirs != Int32.MaxValue && dircont > dirs * stat)
                    {
                        Progress(Convert.ToInt32(stat * 100));
                        stat += 0.1F;
                    }

                }// for

                Progress(100);
            }
            catch (UnauthorizedAccessException exc)
            {
                throw;
            }
            catch (Exception exc)
            {
                Progress(100);
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

            if (data == 0) { return; }

            // Status
            richTextBox1.AppendText("Writing data \n");
            Progress(0);

            // Blocks count
            blocksmb = (1024 * 1000) / bsize;
            blocksgb = (1024 * 1000 * 1000) / bsize;
            blockscont = blocksgb;

            if (data == Int32.MaxValue) { blocks = Int32.MaxValue; }
            else if (listUnit.Text == "Megabytes") { blocks = data * blocksmb; }
            else if (listUnit.Text == "Gigabytes") { blocks = data * blocksgb; }

            // Path
            string dpath = string.Format("{0}{1}0", textPath.Text, prefix);
            string fpath = string.Format("{0}\\{1}0", dpath, prefix);

            try
            {

                // Create temp dir for data file
                Directory.CreateDirectory(dpath);

                // Write data
                using (BinaryWriter writer =
                    new BinaryWriter(File.Open(fpath, FileMode.Create)))
                {
                    //
                    for (int index = 1; index <= blocks; index++)
                    {
                        // Write
                        writer.Write(block, 0, bsize);

                        // Statistic
                        if (blocks != Int32.MaxValue && index > blocks * stat)
                        {
                            Progress(Convert.ToInt32(stat * 100));
                            stat += 0.1F;
                        }

                        if (blocks == Int32.MaxValue && index > blockscont && cont < 10)
                        {
                            cont++;
                            Progress(cont);
                            blockscont += blocksgb;
                        }

                    }// for

                }// using

                // Status
                Progress(100);

            }
            catch (UnauthorizedAccessException exc)
            {
                throw;
            }
            catch (Exception exc)
            {
                Progress(100);
            }


        }// 


        // CleanDirs
        private void CleanDirs()
        {
            // 
            int index = 0;
            float stat = 0.1F;

            //richTextBox1.AppendText("Cleaning \n");
            //Progress(0);

            // Clean dirs
            for (index = 1; index < dircont && dirs > 0; index++)
            {
                Directory.Delete(
                    string.Format("{0}{1}{2}", textPath.Text, prefix, index)
                    );

                //File.Delete(string.Format("{0}{1}{2}", 
                //    textPath.Text, prefix, index));

                //// Statistic
                //if (dircont > dirs * stat)
                //{
                //    Progress(Convert.ToInt32(stat * 100));
                //    stat += 0.1F;
                //}

            }// for

            // Status
            //Progress(100);
        }

        // CleanData
        private void CleanData()
        {
            if (data == 0) { return; }

            // Path
            string dpath = string.Format("{0}{1}0", textPath.Text, prefix);
            string fpath = string.Format("{0}\\00", dpath);

            // Clean data
            File.Delete(fpath);
            Directory.Delete(dpath);

        }// 

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

        // 
        private void Progress(int progress)
        {
            // Status
            progressBar1.Value = progress;
            labelProgress.Text = progress.ToString() + "%";

            // 
            progressBar1.Refresh();
            labelProgress.Refresh();
            richTextBox1.Refresh();
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


