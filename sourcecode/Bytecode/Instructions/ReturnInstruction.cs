using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class ReturnInstruction : AInstruction
    {
        public int ReturnRegister { get; }
        public ReturnInstruction(int regIndex) : base(OpCode.Return)
        {
            ReturnRegister = regIndex;
        }
        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(ReturnRegister);
        }
        public static ReturnInstruction Read(Stream s, IReadConstantSource rcs)
        {
            return new ReturnInstruction(s.ReadInt());
        }
    }
}
