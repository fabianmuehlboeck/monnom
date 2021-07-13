using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class LoadStringConstantInstruction : AInstruction
    {
        public IConstantRef<StringConstant> StringConstant { get; }
        public int Register { get; }
        public LoadStringConstantInstruction(IConstantRef<StringConstant> stringConstant, int register) : base(OpCode.LoadStringConstant)
        {
            StringConstant = stringConstant;
            Register = register;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(StringConstant.ConstantID);
            ws.WriteValue(Register);
        }
        public static LoadStringConstantInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var strconst = rcs.ReferenceStringConstant(s.ReadULong());
            var reg = s.ReadInt();
            return new LoadStringConstantInstruction(strconst, reg);
        }
    }
}
