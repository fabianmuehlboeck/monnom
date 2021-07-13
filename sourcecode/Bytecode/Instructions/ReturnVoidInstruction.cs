using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class ReturnVoidInstruction : AInstruction
    {
        public ReturnVoidInstruction() : base(OpCode.ReturnVoid)
        {

        }

        protected override void WriteArguments(Stream ws)
        {
        }
        public static ReturnVoidInstruction Read(Stream s, IReadConstantSource rcs)
        {
            return new ReturnVoidInstruction();
        }
    }
}
