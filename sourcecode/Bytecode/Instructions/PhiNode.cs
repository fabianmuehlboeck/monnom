using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public class PhiNode : AInstruction
    {
        public readonly IEnumerable<(int, IConstantRef<ITypeConstant>)> Registers;
        public readonly int IncomingCount;
        public PhiNode(IEnumerable<(int, IConstantRef<ITypeConstant>)> registers, int incomingCount) : base(OpCode.PhiNode)
        {
            Registers = registers.ToList();
            IncomingCount = incomingCount;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(IncomingCount);
            ws.WriteValue(Registers.Count());
            foreach(var pair in Registers)
            {
                ws.WriteValue(pair.Item1);
                ws.WriteValue(pair.Item2.ConstantID);
            }
        }

        public static PhiNode Read(Stream s, IReadConstantSource rcs)
        {
            var incomingCount = s.ReadInt();
            var regcount = s.ReadInt();
            List<(int, IConstantRef<ITypeConstant>)> regpairs = new List<(int, IConstantRef<ITypeConstant>)>();
            for(int i=0;i<regcount;i++)
            {
                int fst = s.ReadInt();
                ulong snd = s.ReadULong();
                regpairs.Add((fst, rcs.ReferenceTypeConstant(snd)));
            }
            return new PhiNode(regpairs, incomingCount);
        }
    }
}
