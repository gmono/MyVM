using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
namespace asmcomp
{
    public class Asm
    {
        static Dictionary<string, string> macmap = null;//符号表
        static List<byte> codes = null;
        public static byte[] Trans(string str)
        {
            macmap = new Dictionary<string, string>();
            codes = new List<byte>();
            StringReader reader = new StringReader(str);
            string sym = null;
            while ((sym = reader.ReadLine()) != null)
            {
                if (sym.Trim() == "#symbol") { symbolset(reader); continue; }
                if (sym.Trim() == "#code") { codeset(reader); continue; }

            }
            var v = codes.ToArray();
            codes = null;
            macmap = null;//方便gc释放
            return v;
        }
        /// <summary>
        /// 符号定义读取函数
        /// </summary>
        /// <param name="reader"></param>
        static void symbolset(TextReader reader)
        {
            string str = null;
            while ((str = reader.ReadLine()) != null && str.Trim() != "#end")
            {
                if (str == null) throw new Exception("符号表读取错误");
                string[] strs = str.Split(':');
                if (strs.Length != 2) throw new Exception("符号表读取错误 格式错误");
                macmap.Add(strs[0], strs[1]);//加入符号表
            }
        }
        /// <summary>
        /// 代码读取函数
        /// </summary>
        /// <param name="reader"></param>
        static void codeset(TextReader reader)
        {
            string str = null;
            while ((str = reader.ReadLine()) != null && str.Trim() != "#end")
            {
                if (str == null) throw new Exception("指令读取错误");
                string[] strs = str.Split(' ');//空格分割
                if (strs.Length > 1)
                {
                    for (int i = 0; i < strs.Length; i++)
                    {

                        if (strs[i][0] == '$')
                        {
                            string name = strs[i].Substring(1);
                            if (macmap.ContainsKey(name))
                                strs[i] = macmap[name];
                            else throw new Exception("引用的符号不存在");//不存在这个符号
                        }
                    }
                }
                byte[] bs = createcode(strs);//生成指令
                codes.AddRange(bs);
            }
        }
        struct code
        {
            public byte cds;//指令码
            public string[] types;//参数类型表
            public code(byte c, params string[] tys)
            {
                cds = c;
                types = tys;
            }

        }
        static byte[] createcode(string[] codestr)
        {

            Dictionary<string, code> ds = new Dictionary<string, code>()
            {
                {"rupt",new code(0,"dword") },
                { "leareg",new code(1,"dword","word")},
                {"leamem",new code(2,"word") },
                {"ld",new code(3,"word") },
                {"ldl",new code(4,"dword") },
                {"ldh",new code(5,"dword") },
                {"save", new code(6,"word")}
            };
            //以上为真指令码
            if (ds.ContainsKey(codestr[0]))
            {
                code c = ds[codestr[0]];
                byte[] tbs = new byte[8];
                tbs[0] = c.cds;
                int index = 8;//这是指令码指针 从后面开始
                //i是指令中数据的指针
                for (int i = c.types.Length - 1; i >= 0; --i)
                {
                    string s = c.types[i];//得到类型文本 byte word dword 没有qword
                    int size = 0;
                    switch (s)
                    {
                        case "byte": size = 1; break;
                        case "word": size = 2; break;
                        case "dword": size = 4; break;
                        default:
                            throw new Exception("内部错误");
                    }
                    index -= size;
                    ulong v = getnum(codestr[i+1]); //倒序
                    byte[] vbs = BitConverter.GetBytes(v);
                    for (int t = 0; t < size; t++, index++)
                    {
                        //取其低位装入tbs中
                        tbs[index] = vbs[t];
                    }
                    //装入完毕
                }
                //tbs生成完成
                return tbs;
            }
            else
            {
                List<byte> tblist = new List<byte>();
                switch (codestr[0])
                {
                    case "num":
                        ulong size = getnum(codestr[1]);
                        string temp = codestr[2].Trim();
                        StringBuilder builder = new StringBuilder();
                        bool isdex = false;
                        if (temp[0] == 'd') isdex = true;
                        for (int i = isdex ? 1 : 0; i < temp.Length-1; i+=2)
                        {
                            if (isdex) builder.Append('d');
                            builder.Append(temp.Substring(i, 2));
                        }
                        //以上为把十进制数字 格式化成d10d20d30的标准十进制格式
                        temp = builder.ToString();
                        for (int i = 0; i < (int)size; i++)
                        {
                            int index = temp.Length - i - 1;//过去从后面开始的位置
                            string ns = temp.Substring(index - 1, 2);
                            byte bt = (byte)getnum(ns);
                            tblist.Add(bt);
                        }
                        break;
                    case "data":
                        break;
                    default:
                        throw new Exception("命令错误");
                }
                return tblist.ToArray();
            }

        }
        static ulong getnum(string s)
        {
            ulong num = 0;
            bool isdex = false;
            if (s[0] == 'd')
            {
                isdex = true;
                s = s.Substring(1);
            }
            for (int i = 0; i < s.Length; ++i)
            {
                int index = s.Length - i - 1;
                uint tempnum = 0;
                if (s[index] >= '0' && s[index] <= '9')
                {
                    tempnum = (uint)s[index] - '0';
                }
                else if (s[index] >= 'a' && s[index] <= 'f')
                {

                    tempnum = (uint)s[index] - 'a';
                }
                else if (s[index] >= 'A' && s[index] <= 'F')
                {
                    tempnum = (uint)s[index] - 'A';
                }
                else throw new Exception("数字解析错误");
                if (tempnum > 9 && isdex == true) throw new Exception("数字解析错误");
                num += tempnum * (ulong)Math.Pow(isdex ? 10 : 16, i);//累加
            }
            return num;
        }
    }
}
