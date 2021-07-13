using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class TypeArgRep
    {

        public static void WriteListByteCode(IEnumerable<TypeArgRep> reps, Stream ws)
        {
            using (MemoryStream s = new MemoryStream())
            {
                byte[] buf;

                buf = BitConverter.GetBytes(s.Length);
                ws.Write(buf, 0, buf.Length);
                s.CopyTo(ws);
            }
        }

    }
}
