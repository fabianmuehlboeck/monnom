using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class UnaryOpInstruction : AInstruction
    {
        public int Arg { get; }
        public int Register { get; }
        public Nom.Parser.UnaryOperator Operator { get; }
        public UnaryOpInstruction(Nom.Parser.UnaryOperator op, int arg, int reg) : base(OpCode.UnaryOp)
        {
            Operator = op;
            Arg = arg;
            Register = reg;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteByte((byte)Operator);
            ws.WriteValue(Arg);
            ws.WriteValue(Register);
        }

        public static UnaryOpInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var op = (Nom.Parser.UnaryOperator)s.ReadActualByte();
            var arg = s.ReadInt();
            var reg = s.ReadInt();
            return new UnaryOpInstruction(op, arg, reg);
        }
    }
}
