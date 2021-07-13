using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class CastInstruction : AInstruction
    {
        public int TargetRegister { get; }
        public int ValueRegister { get; }
        public IConstantRef<ITypeConstant> TypeConstant { get; }
        public CastInstruction(int targetReg, int valueReg, IConstantRef<ITypeConstant> type) : base(OpCode.Cast)
        {
            TargetRegister = targetReg;
            ValueRegister = valueReg;
            TypeConstant = type;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(TargetRegister);
            ws.WriteValue(ValueRegister);
            ws.WriteValue(TypeConstant.ConstantID);
        }

        public static CastInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var targetReg = s.ReadInt();
            var valueReg = s.ReadInt();
            var tconst = rcs.ReferenceTypeConstant(s.ReadULong());
            return new CastInstruction(targetReg, valueReg, tconst);
        }
    }
}
