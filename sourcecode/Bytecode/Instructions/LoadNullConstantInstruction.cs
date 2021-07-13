using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class LoadNullConstantInstruction : AInstruction
    {
        public bool Value { get; }
        public int Register { get; }
        public LoadNullConstantInstruction(int register) : base(OpCode.LoadNullConstant)
        {
            Register = register;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Register);
        }
        public static LoadNullConstantInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var reg = s.ReadInt();
            return new LoadNullConstantInstruction(reg);
        }
    }
}
