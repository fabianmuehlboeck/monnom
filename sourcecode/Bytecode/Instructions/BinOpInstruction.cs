using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.TypeChecker;

namespace Nom.Bytecode
{
    public class BinOpInstruction : AInstruction
    {
        public int Left { get; }
        public int Right { get; }
        public int Register { get; }
        public Nom.Parser.BinaryOperator Operator { get; }
        public BinOpInstruction(Nom.Parser.BinaryOperator op, int left, int right, int reg) : base(OpCode.BinOp)
        {
            Operator = op;
            Left = left;
            Right = right;
            Register = reg;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteByte((byte)Operator);
            ws.WriteValue(Left);
            ws.WriteValue(Right);
            ws.WriteValue(Register);
        }

        public static BinOpInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var op = (Nom.Parser.BinaryOperator)s.ReadActualByte();
            var left = s.ReadInt();
            var right = s.ReadInt();
            var reg = s.ReadInt();
            return new BinOpInstruction(op, left, right, reg);
        }
    }
}
