using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class LoadIntConstantInstruction : AInstruction
    {
        public long Value { get; }
        public int Register { get; }
        public LoadIntConstantInstruction(long val, int register) : base(OpCode.LoadIntConstant)
        {
            Value = val;
            Register = register;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Value);
            ws.WriteValue(Register);
        }
        public static LoadIntConstantInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var value = s.ReadLong();
            var reg = s.ReadInt();
            return new LoadIntConstantInstruction(value, reg);
        }
    }
}
