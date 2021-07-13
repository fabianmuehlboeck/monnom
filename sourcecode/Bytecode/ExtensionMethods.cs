using System;
using System.Collections.Generic;
using System.Text;
using Nom.Parser;
using System.IO;
using System.Linq;

namespace Nom.Bytecode
{
    public static class ExtensionMethods
    {
        public static byte[] ReadBytes(this Stream s, int size)
        {
            byte[] ret = new byte[size];
            for(int i=0;i<size;i++)
            {
                int bt = s.ReadByte();
                if(bt<0)
                {
                    throw new InvalidDataException();
                }
                ret[i] = (byte)bt;
            }
            return ret;
        }
        public static byte ReadActualByte(this Stream s)
        {
            return s.ReadBytes(1)[0];
        }
        public static ulong ReadULong(this Stream s)
        {
            return BitConverter.ToUInt64(s.ReadBytes(sizeof(ulong)), 0);
        }
        public static double ReadDouble(this Stream s)
        {
            return BitConverter.ToDouble(s.ReadBytes(sizeof(double)), 0);
        }
        public static long ReadLong(this Stream s)
        {
            return BitConverter.ToInt64(s.ReadBytes(sizeof(long)), 0);
        }
        public static int ReadInt(this Stream s)
        {
            return BitConverter.ToInt32(s.ReadBytes(sizeof(int)), 0);
        }
        public static uint ReadUInt(this Stream s)
        {
            return BitConverter.ToUInt32(s.ReadBytes(sizeof(uint)), 0);
        }

        //public static String ReadASCII(this Stream s, int length)
        //{
        //    return Encoding.ASCII.GetString(s.ReadBytes(length), 0, length);
        //}
        //public static String ReadUTF16(this Stream s, int length)
        //{
        //    Encoding.Unicode.GetDecoder().
        //    return Encoding.ASCII.GetString(s.ReadBytes(length), 0, length);
        //}
        public static String ReadUTF16Str(this Stream s)
        {
            ulong length = s.ReadULong();
            byte[] strbytes = s.ReadBytes((int)length * 2);
            return Encoding.Unicode.GetString(strbytes);
        }

        public static void WriteValue(this Stream s, ulong v)
        {
            byte[] buf = BitConverter.GetBytes(v);
            s.Write(buf, 0, buf.Length);
        }
        public static void WriteValue(this Stream s, double v)
        {
            byte[] buf = BitConverter.GetBytes(v);
            s.Write(buf, 0, buf.Length);
        }
        public static void WriteValue(this Stream s, long v)
        {
            byte[] buf = BitConverter.GetBytes(v);
            s.Write(buf, 0, buf.Length);
        }
        public static void WriteValue(this Stream s, int v)
        {
            byte[] buf = BitConverter.GetBytes(v);
            s.Write(buf, 0, buf.Length);
        }
        public static void WriteValue(this Stream s, uint v)
        {
            byte[] buf = BitConverter.GetBytes(v);
            s.Write(buf, 0, buf.Length);
        }
        //public static void WriteASCII(this Stream s, String str)
        //{
        //    byte[] buf = Encoding.ASCII.GetBytes(str);
        //    s.Write(buf, 0, buf.Length);
        //}
        //public static void WriteUTF16(this Stream s, String str)
        //{
        //    byte[] buf = Encoding.Unicode.GetBytes(str);
        //    s.Write(buf, 0, buf.Length);
        //}
        public static void WriteUTF16Str(this Stream s, String str)
        {
            byte[] buf = Encoding.Unicode.GetBytes(str);
            ulong buflen = (ulong)buf.Length;
            byte[] lenbuf = BitConverter.GetBytes(buflen/2);
            s.Write(lenbuf, 0, lenbuf.Length);
            s.Write(buf, 0, buf.Length);
        }

        public static void WriteBytes(this Stream s, IEnumerable<byte> bytes)
        {
            foreach(byte b in bytes)
            {
                s.WriteByte(b);
            }
        }

        public static IEnumerable<bool> Decompress(this Stream s, int flagcount)
        {
            while(flagcount>0)
            {
                int read = s.ReadByte();
                if(read<0)
                {
                    throw new InvalidDataException();
                }
                uint b = (uint)read;
                for(int i = 7;i>=0&&flagcount>0;i--,flagcount--)
                {
                    uint bit = (b >> i) & 1;
                    if (bit == 1)
                    {
                        yield return true;
                    }
                    else
                    {
                        yield return false;
                    }
                }
            }
        }

        public static IEnumerable<byte> Compress(this IEnumerable<bool> bools)
        {
            int buf = 0;
            int cnt = 0;
            foreach(bool b in bools)
            {
                if(b)
                {
                    buf++;
                }
                cnt++;
                if (cnt == 8)
                {
                    yield return (byte)cnt;
                    buf = 0;
                    cnt = 0;
                }
                else
                {
                    buf = buf << 1;
                }
            }
            if(cnt>0)
            {
                yield return (byte)cnt;
            }
        }
    }
}
