using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class CallCheckedConstructorInstruction : ACallInstruction
    {
        public IConstantRef<SuperClassConstant> Class { get; }
        public int ResultRegister { get; }
        public CallCheckedConstructorInstruction(IConstantRef<SuperClassConstant> cls, IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, int resultRegister) : base(typeArguments, argumentRegisters,OpCode.CallConstructor)
        {
            Class = cls;
            ResultRegister = resultRegister;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(ResultRegister);
            ws.WriteValue(Class.ConstantID);
            ws.WriteValue(TypeArguments.ConstantID);
        }

        public static CallCheckedConstructorInstruction Read(Stream s, IReadConstantSource rcs, IEnumerable<int> argRegs)
        {
            int resultReg = s.ReadInt();
            var cls = rcs.ReferenceSuperClassConstant(s.ReadULong());
            var targs = rcs.ReferenceTypeListConstant(s.ReadULong());
            return new CallCheckedConstructorInstruction(cls, targs, argRegs, resultReg);
        }
    }
}
