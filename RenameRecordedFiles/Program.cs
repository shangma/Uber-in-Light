using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RenameRecordedFiles
{

    enum Decodeing
    {
        FFT,
        FFT_NO_RANDOM,
        CROSS_CORRELATION
    }
    class Program
    {
        static int getMode(string file)
        {
            if (file.ToLower().Contains("ampdiff"))
            {
                return 1;
            }
            if (file.ToLower().Contains("splitamp"))
            {
                return 5;
            }
            if(file.ToLower().Contains("split"))
            {
                return 4;
            }
            return 0;
        }
        static int getCorrectionCode(string new_name)
        {
            int correction_code = 0;
            if (new_name.ToLower().Contains("hamming"))
            {
                correction_code = 1;
            }
            else if (new_name.ToLower().Contains("solomon"))
            {
                correction_code = 2;
            }
            return correction_code;
        }
        static int[] getFreq(string new_name)
        {
            int zero = 12;
            int one = 8;
            string[] parts = new_name.ToLower().Split(new string[] { "_" }, StringSplitOptions.RemoveEmptyEntries);
            // get zero
            int j = 0;
            for (; j < parts.Length; j++)
            {
                if (parts[j].EndsWith("hz"))
                {
                    if (int.TryParse(parts[j].Substring(0, parts[j].Length - 2), out zero))
                    {
                        j++;
                        break;
                    }
                }
            }
            // get one 
            for (; j < parts.Length; j++)
            {
                if (parts[j].EndsWith("hz"))
                {
                    if (int.TryParse(parts[j].Substring(0, parts[j].Length - 2), out one))
                    {
                        break;
                    }
                }
            }
            return (new int[] { zero, one });
        }
        static string getRandFileName(string new_name)
        {
            int last = new_name.IndexOf("rand_");
            int first = new_name.Substring(0, last).LastIndexOf("_") + 1;

            return new_name.Substring(first, last - first) + ".rand";
        }
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
            for (int i = 0; i < mp4Files.Length; i++)
            {
                FileInfo avif = new FileInfo(aviFiles[i % aviFiles.Length]);
                FileInfo mp4f = new FileInfo(mp4Files[i]);
                string new_name = mp4f.Name + "_" + avif.Name;
                new_name = new_name.Replace(".avi", "");
                new_name = new_name.Replace(".mp4", "");
                new_name += ".mp4";
                File.Copy(mp4Files[i], new_name);
                //File.Delete(mp4Files[i]);
                int mode = getMode(new_name); /// 0 -  normal, 1 -> AmpDifference

                int correction_code = getCorrectionCode(new_name);
                int[] freq = getFreq(new_name);
                sw.WriteLine(@"VLC_tester.exe -decode {6} -r -zero {4} -one {5} -t {7} -if {0}\\{1} -roi 1 -m {2} -ec {3} > {0}\\{1}.txt",
                    dinf.Name, new_name, mode, correction_code, freq[0], freq[1], (int)Decodeing.CROSS_CORRELATION, 
                    getRandFileName(new_name));
            }
            sw.Close();
            //if (mp4Files.Length == 0)
            {
                sw = new StreamWriter("r_" + dinf.Name + "_gt.bat");
                // then use the original AVI files as the test
                for (int i = 0; i < aviFiles.Length; i++)
                {
                    FileInfo finf = new FileInfo(aviFiles[i]);
                    string new_name = finf.Name;
                    int mode = getMode(new_name);
                    int correction_code = getCorrectionCode(new_name);
                    int[] freq = getFreq(new_name);
                    sw.WriteLine(@"VLC_tester.exe -decode {6} -r -zero {4} -one {5} -t {7} -if {0}\\{1} -roi 1 -m {2} -ec {3} > {0}\\{1}.txt",
                   dinf.Name, new_name, mode, correction_code, freq[0], freq[1], (int)Decodeing.CROSS_CORRELATION,
                   getRandFileName(new_name));
                }
            }
            sw.Close();
        }
    }
}
