
/*
TITLE:			Overwrite Program
DESCRIPTION:	Overwrite empty space on disk data and metadata.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
VERSION:		1.6 2019-11-21

*/


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.Threading;
using System.Diagnostics;


namespace overwrite_ui
{
    public partial class overwrite : Form
    {

        // 
        const bool SAFE_WRITE = true;       // Check if file exists before overwrite
        const string FILE_SUFFIX = "x";     // File suffix
        const string FILE_SUFFIX2 = "Y";    // File suffix
        int blocksize = 4096;               // Block size

        int files = 0;          // Number of files
        int filecont = 0;       // Counter for write and clean
        int data = 0;           // Quantity of data to write
        string path;            // Path where to overwrite
        byte[] blockbuf;        // Block buffer

        // 
        public overwrite()
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
            textPath.Text = Path.GetTempPath();

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

                // Prepare 
                Prepare();

                // Write files
                WriteFiles();

                // Write data
                WriteData();
                if (!checkTest.Checked) { CleanData(); }

                // Done
                stopwatch.Stop();
                WriteTime(stopwatch);
                richTextBox1.AppendText(" Done. \n");


            }
            catch (Exception exc)
            {
                richTextBox1.AppendText(" " + exc.Message);
            }

        }// 


        // Initialize parameters
        private void Prepare()
        {
            blocksize = Convert.ToInt32(listBlock.Text);
            blockbuf = new byte[blocksize];
            files = Convert.ToInt32(listFiles.Text);

            if (listData.Text == "All") { data = Int32.MaxValue; }
            else { data = Convert.ToInt32(listData.Text); }

            // Set path
            path = textPath.Text;
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                if (!path.EndsWith("/") && !path.EndsWith("\\"))
                {
                    path += "/";
                }
            }
            else
            {
                if (!path.EndsWith("/"))
                {
                    path += "/";
                }
            }


            // Set data content
            if (listContent.Text == "Zero")
            {
                for (int index = 0; index < blocksize; index++)
                {
                    blockbuf[index] = 0;
                }
            }
            else if (listContent.Text == "One")
            {
                for (int index = 0; index < blocksize; index++)
                {
                    blockbuf[index] = 255;
                }
            }
            else // if (listContent.Text == "Random")
            {
                Random rand = new Random(DateTime.Now.Millisecond);
                for (int index = 0; index < blocksize; index++)
                {
                    blockbuf[index] = (byte)rand.Next(1, 255);
                }
            }

        }// 


        // Write files
        private void WriteFiles()
        {
            if (files == 0) { return; }

            richTextBox1.AppendText(" Writing metadata \n");
            Progress(0);

            WriteFiles(FILE_SUFFIX, true);
            if (!checkTest.Checked) { CleanFiles(FILE_SUFFIX); }

            WriteFiles(FILE_SUFFIX2, false);
            if (!checkTest.Checked) { CleanFiles(FILE_SUFFIX2); }

        }// 


        // Write files
        private void WriteFiles(string suffix, bool data)
        {
            string fpath = "";
            float prog = 0.1F; // Progress
            byte[] empty = new byte[0];

            try
            {
                // Write files
                for (filecont = 1; filecont <= files; filecont++)
                {
                    // Write
                    if (data)
                    {
                        // Create file blockbuf size
                        fpath = string.Format("{0}{1}{2}", path, filecont, suffix);
                        SafeCheck(fpath);
                        WriteFile(fpath, blockbuf);
                    }
                    else
                    {
                        // Create empty file
                        fpath = string.Format("{0}{1}{2}", path, filecont, suffix);
                        SafeCheck(fpath);
                        WriteFile(fpath, empty);
                    }

                    // Statistic
                    if (data && filecont > files * prog)
                    {
                        Progress(Convert.ToInt32(prog * 100));
                        prog += 0.1F;
                    }

                }// for


            }
            catch (UnauthorizedAccessException exc)
            {
                throw;
            }
            finally
            {
                Progress(100);
            }

        }// 


        // WriteFile
        private void WriteFile(string path, byte[] data)
        {

            using (FileStream stream = new FileStream(
                path,
                FileMode.Create,
                FileAccess.Write,
                FileShare.None, 4096,
                FileOptions.WriteThrough))
            {
                using (BinaryWriter writer = new BinaryWriter(stream))
                {
                    writer.Write(data);
                }// using
            }// using

        }


        // Write data
        private void WriteData()
        {
            int blocks = 0;         // Number of blocks to write
            int blocksmb = 0;       // Blocks per megabyte
            int blocksgb = 0;       // Blocks per gigabyte
            int blockscont = 0;     // Blocks count for statistic
            int blockscont2 = 0;    // Blocks count for statistic
            float prog = 0.1F;      // Progress

            if (data == 0) { return; }

            // Status
            richTextBox1.AppendText(" Writing data \n");
            Progress(0);

            // Blocks count
            blocksmb = (1024 * 1000) / blocksize;
            blocksgb = (1024 * 1000 * 1000) / blocksize;
            blockscont = blocksgb;

            if (data == Int32.MaxValue) { blocks = Int32.MaxValue; }
            else if (listUnit.Text == "Megabytes") { blocks = data * blocksmb; }
            else if (listUnit.Text == "Gigabytes") { blocks = data * blocksgb; }

            // Path
            string fpath = string.Format("{0}{1}{2}", path, 0, FILE_SUFFIX);
            SafeCheck(fpath);

            try
            {
                // Write data
                //using (BinaryWriter writer =
                //    new BinaryWriter(File.Open(fpath, FileMode.Create)))
                using (FileStream file =
                       File.Create(fpath, 1024 * 1000, FileOptions.WriteThrough))
                {
                    using (BinaryWriter writer = new BinaryWriter(file))
                    {
                        //
                        for (int index = 1; index <= blocks; index++)
                        {
                            // Write
                            writer.Write(blockbuf, 0, blocksize);

                            // Statistic
                            if (blocks != Int32.MaxValue && index > blocks * prog)
                            {
                                Progress(Convert.ToInt32(prog * 100));
                                prog += 0.1F;
                            }

                            if (blocks == Int32.MaxValue && index > blockscont && blockscont2 < 10)
                            {
                                blockscont += blocksgb;
                                blockscont2++;
                                Progress(blockscont2);
                            }

                        }// for

                    }// using
                }// using

                Progress(100);

            }
            catch (UnauthorizedAccessException exc)
            {
                throw;
            }
            catch (IOException exc)
            {
                // Disk full, no exception
                Progress(100);

            }
            finally
            {
            }


        }// 


        // Clean directories
        private void CleanFiles(string suffix)
        {
            // 
            int index = 0;
            float prog = 0.1F;

            //richTextBox1.AppendText("Cleaning \n");
            //Progress(0);
            if (files < 1) { return; }

            // Clean files
            for (index = 1; index < filecont; index++)
            {
                string fpath = string.Format("{0}{1}{2}", path, index, suffix);
                File.Open(fpath, FileMode.Truncate).Close();
                File.Delete(fpath);

                //// Statistic
                //if (filecont > files * prog)
                //{
                //    Progress(Convert.ToInt32(prog * 100));
                //    prog += 0.1F;
                //}

            }// for

            // Status
            //Progress(100);
        }

        // Clean data
        private void CleanData()
        {
            if (data == 0) { return; }

            // Clean data
            string fpath = string.Format("{0}{1}{2}", path, 0, FILE_SUFFIX);
            File.Open(fpath, FileMode.Truncate).Close();
            File.Delete(fpath);

        }// 


        // Check if file exists befor overwrite
        private void SafeCheck(string path)
        {
            if (SAFE_WRITE && !checkTest.Checked && File.Exists(path))
            {
                string msg = string.Format(
                    "File exists, clean file before overwrite, safe write is on. \n{0} \n",
                    path
                    );

                throw new ApplicationException(msg);
            }
        }


        // WriteTime
        private void WriteTime(Stopwatch stopwatch)
        {
            string time = string.Format(
                " Time: {0:00}:{1:00}:{2:00}.{3} \n",
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


