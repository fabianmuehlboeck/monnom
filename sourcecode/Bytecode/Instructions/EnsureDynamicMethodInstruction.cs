using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class EnsureDynamicMethodInstruction : AInstruction
    {
        public IConstantRef<StringConstant> MethodName { get; }
        public int ReceiverRegister { get; }
        public EnsureDynamicMethodInstruction(IConstantRef<StringConstant> methodName, int receiver) : base(OpCode.EnsureDynamicMethod)
        {
            MethodName = methodName;
            ReceiverRegister = receiver;
        }
        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(MethodName.ConstantID);
            ws.WriteValue(ReceiverRegister);
        }
    }
}
