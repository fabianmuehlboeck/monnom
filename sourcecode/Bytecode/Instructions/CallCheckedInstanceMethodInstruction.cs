using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class CallCheckedInstanceMethodInstruction : ACallInstruction
    {
        public IConstantRef<MethodConstant> Method { get; }
        public int ResultRegister { get; }
        public int ReceiverRegister { get; }
        public CallCheckedInstanceMethodInstruction(IConstantRef<MethodConstant> method, IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, int resultRegister, int receiverRegister) : base(typeArguments, argumentRegisters, OpCode.InvokeCheckedInstance)
        {
            Method = method;
            ResultRegister = resultRegister;
            ReceiverRegister = receiverRegister;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(Method.ConstantID);
            ws.WriteValue(TypeArguments.ConstantID);
            ws.WriteValue(ResultRegister);
            ws.WriteValue(ReceiverRegister);
        }

        public static CallCheckedInstanceMethodInstruction Read(Stream s, IReadConstantSource rcs, IEnumerable<int> argRegs)
        {
            var method = rcs.ReferenceMethodConstant(s.ReadULong());
            var targs = rcs.ReferenceTypeListConstant(s.ReadULong());
            int resultReg = s.ReadInt();
            int receiverReg = s.ReadInt();
            return new CallCheckedInstanceMethodInstruction(method, targs, argRegs, resultReg, receiverReg);
        }
    }
}
