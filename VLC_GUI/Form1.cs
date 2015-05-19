using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VLC_GUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void buttonSelect_Click(object sender, EventArgs e)
        {
            // Create an instance of the open file dialog box.
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            // Set filter options and filter index.
            openFileDialog1.Filter = "Text Files (.avi)|*.avi|All Files (*.*)|*.*";
            openFileDialog1.FilterIndex = 1;

            openFileDialog1.Multiselect = false;

            // Call the ShowDialog method to show the dialog box.
            DialogResult userClickedOK = openFileDialog1.ShowDialog();

            // Process input if the user clicked OK.
            if (userClickedOK == System.Windows.Forms.DialogResult.OK)
            {
                textBoxVideoName.Text = openFileDialog1.FileName;
            }
        }
        private void createVideo(string outputVideoName)
        {
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            FileInfo finf = new FileInfo(textBoxVideoName.Text);
            startInfo.WorkingDirectory = finf.DirectoryName;
            string videoName = finf.Name.Substring(finf.Name.LastIndexOf("\\") + 1);
            startInfo.FileName = "VLC_tester.exe";
            startInfo.Arguments = "-s -t \"" + textBoxText.Text + "\" -ec 0 -sideB 1 -sideA 1 -if " +
                videoName
                + " -v -symbols old.symbol -m 9 -fps 60 -time 500 -full 1 -codec XVID -of " + outputVideoName + ".avi";
            //MessageBox.Show(startInfo.Arguments);
            process.StartInfo = startInfo;
            process.Start();
            process.WaitForExit();
        }
        private void playVideo(string outputVideoName)
        {
            FileInfo finf = new FileInfo(textBoxVideoName.Text);
            string directory = finf.DirectoryName;
            string[] files = Directory.GetFiles(directory, "*" + outputVideoName + ".avi");
            Array.Sort(files);
            // play the last file
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            startInfo.WorkingDirectory = directory;
            string videoName = finf.Name.Substring(finf.Name.LastIndexOf("\\") + 1);
            startInfo.FileName = "mpc-hc.exe";
            startInfo.Arguments = files[files.Length - 1] + " /fullscreen";
            process.StartInfo = startInfo;
            process.Start();
            process.WaitForExit();
        }
        private void buttonSend_Click(object sender, EventArgs e)
        {
            string name = "VLCGUI";
            createVideo(name);
            playVideo(name);
        }
    }
}
