using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
        static string getFreq(string new_name)
        {
            // check if symbols file used
            if (new_name.ToLower().Contains("symbol_"))
            {
                int last = new_name.ToLower().IndexOf("symbol_");
                int first = new_name.Substring(0,last - 1).ToLower().LastIndexOf("_") + 1;
                return "-symbols " + new_name.Substring(first, last - first) + ".symbol";
            }
            else
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
                return ("-zero " + zero.ToString() + "-one" + one.ToString());
            }
        }
        static string getRandFileName(string new_name)
        {
            int last = new_name.IndexOf("rand_");
            int first = new_name.Substring(0, last).LastIndexOf("_") + 1;

            return new_name.Substring(first, last - first) + ".rand";
        }
        static string getSymbolTime(string new_name)
        {
            Regex reg = new Regex(@"_\d+ms_");
            Match m = reg.Match(new_name);
            return m.Value.Substring(1,m.Value.Length - 4);
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
                string new_name = Path.GetFileNameWithoutExtension(mp4f.Name);
                if (!mp4f.Name.Contains(Path.GetFileNameWithoutExtension(avif.Name)))
                {
                    new_name += "_" + Path.GetFileNameWithoutExtension(avif.Name);
                }
                new_name += ".mp4";
                try
                {
                    File.Move(mp4Files[i], new_name);
                }
                catch(Exception)
                {

                }
                //File.Delete(mp4Files[i]);
                int mode = getMode(avif.Name); /// 0 -  normal, 1 -> AmpDifference

                int correction_code = getCorrectionCode(avif.Name);
                string symbols = getFreq(avif.Name);
                string time = getSymbolTime(avif.Name);
                sw.WriteLine(@"VLC_tester.exe -decode {5} -r {4} -t {6} -if {0}\\{1} -roi 1 -m {2} -ec {3} -time {7} > {0}\\{1}.txt",
                    dinf.Name, new_name, mode, correction_code, symbols, (int)Decodeing.CROSS_CORRELATION, 
                    getRandFileName(new_name),time);
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
                    string symbols = getFreq(new_name);
                    string time = getSymbolTime(new_name);
                    sw.WriteLine(@"VLC_tester.exe -decode {5} -r {4} -t {6} -if {0}\\{1} -roi 1 -m {2} -ec {3} -time {7} > {0}\\{1}.txt",
                    dinf.Name, new_name, mode, correction_code, symbols, (int)Decodeing.CROSS_CORRELATION,
                    getRandFileName(new_name), time);
                }
            }
            sw.Close();
        }
    }
}
