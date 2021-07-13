using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class DebugInstruction : AInstruction
    {
        public IConstantRef<StringConstant> Message { get; }
        public DebugInstruction(IConstantRef<StringConstant> msg):base(OpCode.Debug)
        {
            Message = msg;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Message.ConstantID);
        }

        public static DebugInstruction Read(Stream s, IReadConstantSource rcs)
        {
            return new DebugInstruction(rcs.ReferenceStringConstant(s.ReadULong()));
        }
    }
}
