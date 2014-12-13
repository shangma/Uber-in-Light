using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RandomBitsGenerator
{
    class Program
    {
        static void Main(string[] args)
        {
            for (int side = 2; side < 200; side <<= 1)
            {
                StreamWriter sw = new StreamWriter(string.Format("{0}x{0}.rand", side));
                int length = side * side * 30; // 30 seconds
                Random r = new Random((int)DateTime.Now.Ticks);
                for (int i = 0; i < length; i++)
                {
                    sw.Write(r.Next(2));
                }
                sw.Close();
            }
        }
    }
}
