using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    class ErrorInstruction : AInstruction
    {
        public int Register { get; }
        public ErrorInstruction(int register) : base(OpCode.Error)
        {
            Register = register;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Register);
        }
        public static ErrorInstruction Read(Stream s, IReadConstantSource rcs)
        {
            return new ErrorInstruction(s.ReadInt());
        }
    }
}
