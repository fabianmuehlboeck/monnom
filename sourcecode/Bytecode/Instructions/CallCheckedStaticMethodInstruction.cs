using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class CallCheckedStaticMethodInstruction : ACallInstruction
    {
        public IConstantRef<StaticMethodConstant> StaticMethod { get; }
        public int ResultRegister { get; }
        public CallCheckedStaticMethodInstruction(IConstantRef<StaticMethodConstant> staticMethod, IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, int resultRegister) : base(typeArguments, argumentRegisters, OpCode.CallCheckedStatic)
        {
            StaticMethod = staticMethod;
            ResultRegister = resultRegister;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(StaticMethod.ConstantID);
            ws.WriteValue(TypeArguments.ConstantID);
            ws.WriteValue(ResultRegister);
        }

        public static CallCheckedStaticMethodInstruction Read(Stream s, IReadConstantSource rcs, IEnumerable<int> argRegs)
        {
            var method = rcs.ReferenceStaticMethodConstant(s.ReadULong());
            var targs = rcs.ReferenceTypeListConstant(s.ReadULong());
            int resultReg = s.ReadInt();
            return new CallCheckedStaticMethodInstruction(method, targs, argRegs, resultReg);
        }
    }
}
