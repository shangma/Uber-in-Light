using System;

namespace LCS
{
    class Program
    {
        static void Main(string[] args)
        {
            string abcde = "1000011001000110110001100010011010100110";
            string abcdefghijk = "100001100100011011000110001001101010011001100110111001100001011010010110010101101101011000110";
            string abcdefghijklm = "10000110010001101100011000100110101001100110011011100110000101101001011001010110110101100011011010110110";
            string test =           "00000010001010110";
            LCS(test, abcde);
            //Console.WriteLine("size = {0}", result.Length);
            Console.ReadKey();
        }

        public static void LCS(string str1, string str2)
        {
            int[,] l = new int[str1.Length + 1, str2.Length + 1];
            //int lcs = -1;
            
            for (int i = 0; i <= str1.Length; i++)
            {
                l[i,0] = 0;
            }
            for (int j = 0; j <= str2.Length; j++)
            {
                l[0,j] = 0;
            }
            Console.WriteLine("{0} and {1}", str1.Length, str2.Length);
            for(int i = 1;i <= str1.Length;i++)
            {
                for(int j = 1;j <= str2.Length;j++)
                {
                    l[i, j] = Math.Max(l[i - 1, j - 1], Math.Max(l[i - 1, j], l[i, j - 1]));
                    if (str1[i - 1] == str2[j - 1])
                    {
                        l[i, j] = Math.Max(l[i - 1, j - 1] + 1, Math.Max(l[i - 1, j], l[i, j - 1]));
                    }

                }
            }

            Console.WriteLine("Longest Common SubString Length = {0}", l[str1.Length,str2.Length]);
            
        } 
    }
}