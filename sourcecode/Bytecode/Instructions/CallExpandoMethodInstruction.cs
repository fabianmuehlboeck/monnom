using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    class CallExpandoMethodInstruction : ACallInstruction
    {
        public int ReceiverRegister { get; }
        public IConstantRef<StringConstant> MethodNameConstant { get; }
        public IConstantRef<TypeListConstant> TypeArgumentsConstant { get; }
        public int ResultRegister { get; }
        public CallExpandoMethodInstruction(int receiverRegister, IConstantRef<StringConstant> methodName, IConstantRef<TypeListConstant> typeArguments, IEnumerable<int> argumentRegisters, int resultRegister) : base(typeArguments, argumentRegisters, OpCode.CallDispatchBest)
        {
            ReceiverRegister = receiverRegister;
            MethodNameConstant = methodName;
            TypeArgumentsConstant = typeArguments;
            ResultRegister = resultRegister;
        }

        //public override ulong InstructionCount => base.InstructionCount + 1;
        //public override void WriteByteCode(Stream ws)
        //{
        //    ws.WriteByte((byte)OpCode.EnsureDynamicMethod);
        //    ws.WriteValue(MethodNameConstant.ConstantID);
        //    ws.WriteValue(ReceiverRegister);
        //    base.WriteByteCode(ws);
        //}

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(ResultRegister);
            ws.WriteValue(ReceiverRegister);
            ws.WriteValue(MethodNameConstant.ConstantID);
            ws.WriteValue(TypeArgumentsConstant.ConstantID);
        }
        public static CallExpandoMethodInstruction Read(Stream s, IReadConstantSource rcs, IEnumerable<int> argRegs)
        {
            int resultReg = s.ReadInt();
            int receiverReg = s.ReadInt();
            var methodName = rcs.ReferenceStringConstant(s.ReadULong());
            var targs = rcs.ReferenceTypeListConstant(s.ReadULong());
            return new CallExpandoMethodInstruction(receiverReg, methodName, targs, argRegs, resultReg);
        }
    }
}
