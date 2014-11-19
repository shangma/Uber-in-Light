using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RenameRecordedFiles
{
    class Program
    {
        static void Main(string[] args)
        {
            string currentDirectory = Environment.CurrentDirectory;
            string[] aviFiles = Directory.GetFiles(currentDirectory, "*.avi");
            string[] mp4Files = Directory.GetFiles(currentDirectory, "*.mp4");
            Array.Sort(aviFiles);
            //Array.Reverse(aviFiles);
            Array.Sort(mp4Files);
            DirectoryInfo dinf = new DirectoryInfo(currentDirectory);
            StreamWriter sw = new StreamWriter("r_" + dinf.Name + ".bat");
            for(int i = 0;i < mp4Files.Length;i++)
            {
                FileInfo avif = new FileInfo(aviFiles[i % aviFiles.Length]);
                FileInfo mp4f = new FileInfo(mp4Files[i]);
                string new_name = mp4f.Name + "_" + avif.Name;
                new_name = new_name.Replace(".avi", "");
                new_name = new_name.Replace(".mp4", "");
                new_name += ".mp4";
                File.Copy(mp4Files[i], new_name);
                //File.Delete(mp4Files[i]);
                int mode = 0; /// 0 -  normal, 1 -> AmpDifference
                if(new_name.ToLower().Contains("ampdiff"))
                {
                    mode = 1;
                }
                sw.WriteLine(@"VLC_tester.exe -r -zero 12 -one 8 -t {0}\\test.rand -if {0}\\{1} -roi 1 -m {2} > {0}\\{1}.txt", dinf.Name, new_name, mode);
            }
            sw.Close();
        }
    }
}
