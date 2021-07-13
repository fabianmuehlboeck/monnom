using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    class LoadFloatConstantInstruction : AInstruction
    {
        public double Value { get; }
        public int Register { get; }
        public LoadFloatConstantInstruction(double val, int register) : base(OpCode.LoadFloatConstant)
        {
            Value = val;
            Register = register;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Value);
            ws.WriteValue(Register);
        }
        public static LoadFloatConstantInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var value = s.ReadDouble();
            var reg = s.ReadInt();
            return new LoadFloatConstantInstruction(value, reg);
        }
    }
}
