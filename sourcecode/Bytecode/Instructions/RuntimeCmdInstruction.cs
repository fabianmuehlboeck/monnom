using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    class RuntimeCmdInstruction : AInstruction
    {
        public IConstantRef<StringConstant> Cmd { get; }
        public RuntimeCmdInstruction(IConstantRef<StringConstant> cmd) : base(OpCode.RTCmd)
        {
            Cmd = cmd;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Cmd.ConstantID);
        }

        public static RuntimeCmdInstruction Read(Stream s, IReadConstantSource rcs)
        {
            return new RuntimeCmdInstruction(rcs.ReferenceStringConstant(s.ReadULong()));
        }
    }
}