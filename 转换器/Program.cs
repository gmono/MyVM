using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using asmcomp;
namespace 转换器
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("MVM汇编器（基础 调试）\n");
            string path = null;
            if (args.Length >= 1) path = args[0];
            else
            {
                Console.Write("请输入编译文件地址:");
                path = Console.ReadLine();
            }
            if(new FileInfo(path).Exists==false) { Console.Write("文件不存在！"); }
            StreamReader stream = new StreamReader(File.OpenRead(path));
            StringBuilder builder = new StringBuilder();
            string nowline = null;
            while((nowline=stream.ReadLine())!=null)
            {
                if (nowline[0] == nowline[1] && nowline[1] == '/') continue;
                builder.AppendLine(nowline);
            }
            string bins = builder.ToString();
            byte[] data = Asm.Trans(bins);

            if (args.Length >= 2) path = args[1];
            else
            {
                Console.Write("请输入写入的文件地址：");
                path = Console.ReadLine();
            }
            FileStream str = File.OpenWrite(path);
            str.Write(data, 0, data.Length);
            Console.WriteLine("写入成功结束！");
        }
    }
}
