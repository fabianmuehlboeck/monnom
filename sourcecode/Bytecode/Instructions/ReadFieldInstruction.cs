using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

namespace Nom.Bytecode
{
    class ReadFieldInstruction : AInstruction
    {
        public int TargetRegister { get; }
        public int ReceiverRegister { get; }
        public IConstantRef<IStringConstant> FieldName { get; }
        public IConstantRef<IConstant> ReceiverClass { get; }
        public ReadFieldInstruction(int targetReg, int receiver, IConstantRef<IStringConstant> fieldName, IConstantRef<IConstant> receiverClass) : base(OpCode.ReadField)
        {
            TargetRegister = targetReg;
            ReceiverRegister = receiver;
            FieldName = fieldName;
            ReceiverClass = receiverClass;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(TargetRegister);
            ws.WriteValue(ReceiverRegister);
            ws.WriteValue(FieldName.ConstantID);
            ws.WriteValue(ReceiverClass.ConstantID);
        }

        public static ReadFieldInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var targetReg = s.ReadInt();
            var receiverReg = s.ReadInt();
            var fieldName = rcs.ReferenceStringConstant(s.ReadULong());
            var receiverClass = rcs.ReferenceConstant(s.ReadULong());
            return new ReadFieldInstruction(targetReg, receiverReg, fieldName, receiverClass);
        }
    }
}
