using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public class BranchInstruction : AInstruction
    {
        public readonly int Target;
        public readonly IEnumerable<(int, int)> Incomings;
        public BranchInstruction(int phinode, IEnumerable<(int,int)> incomings) : base(OpCode.Branch)
        {
            Incomings = incomings.ToList();
            Target = phinode;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Target);
            ws.WriteValue(Incomings.Count());
            foreach(var pair in Incomings)
            {
                ws.WriteValue(pair.Item1);
                ws.WriteValue(pair.Item2);
            }
        }

        public static BranchInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var target = s.ReadInt();
            var incomingsCount = s.ReadInt();
            List<(int, int)> incomings = new List<(int, int)>();
            for(int i=0;i< incomingsCount; i++)
            {
                var fst = s.ReadInt();
                var snd = s.ReadInt();
                incomings.Add((fst, snd));
            }
            return new BranchInstruction(target, incomings);
        }
    }
}
