using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public class CondBranchInstruction : AInstruction
    {
        public readonly int Condition;
        public readonly int ThenNode;
        public readonly int ElseNode;
        public readonly IEnumerable<(int, int)> ThenIncomings;
        public readonly IEnumerable<(int, int)> ElseIncomings;
        public CondBranchInstruction(int condition, int thenNode, int elseNode, IEnumerable<(int,int)> thenIncomings, IEnumerable<(int,int)> elseIncomings) : base(OpCode.CondBranch)
        {
            Condition = condition;
            ThenNode = thenNode;
            ElseNode = elseNode;
            ThenIncomings = thenIncomings;
            ElseIncomings = elseIncomings;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Condition);
            ws.WriteValue(ThenNode);
            ws.WriteValue(ElseNode);
            ws.WriteValue(ThenIncomings.Count());
            ws.WriteValue(ElseIncomings.Count());
            foreach (var pair in ThenIncomings)
            {
                ws.WriteValue(pair.Item1);
                ws.WriteValue(pair.Item2);
            }
            foreach (var pair in ElseIncomings)
            {
                ws.WriteValue(pair.Item1);
                ws.WriteValue(pair.Item2);
            }
        }
        public static CondBranchInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var condition = s.ReadInt();
            var thenNode = s.ReadInt();
            var elseNode = s.ReadInt();
            var thenIncomingsCount = s.ReadInt();
            var elseIncomingsCount = s.ReadInt();
            List<(int, int)> thenIncomings = new List<(int, int)>();
            List<(int, int)> elseIncomings = new List<(int, int)>();
            for (int i = 0; i < thenIncomingsCount; i++)
            {
                var fst = s.ReadInt();
                var snd = s.ReadInt();
                thenIncomings.Add((fst, snd));
            }
            for (int i = 0; i < elseIncomingsCount; i++)
            {
                var fst = s.ReadInt();
                var snd = s.ReadInt();
                elseIncomings.Add((fst, snd));
            }
            return new CondBranchInstruction(condition, thenNode, elseNode, thenIncomings, elseIncomings);
        }
    }
}
