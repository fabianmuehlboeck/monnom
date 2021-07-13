using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class WriteFieldInstruction : AInstruction
    {
        public int ValueRegister { get; }
        public int ReceiverRegister { get; }
        public IConstantRef<IStringConstant> FieldName { get; }
        public IConstantRef<IClassConstant> ReceiverClass { get; }
        public WriteFieldInstruction(int valueReg, int receiver, IConstantRef<IStringConstant> fieldName, IConstantRef<IClassConstant> receiverClass) : base(OpCode.WriteField)
        {
            ValueRegister = valueReg;
            ReceiverRegister = receiver;
            FieldName = fieldName;
            ReceiverClass = receiverClass;
        }

        protected override void WriteArguments(Stream ws)
        {
            ws.WriteValue(ReceiverRegister);
            ws.WriteValue(ValueRegister);
            ws.WriteValue(FieldName.ConstantID);
            ws.WriteValue(ReceiverClass.ConstantID);
        }
        public static WriteFieldInstruction Read(Stream s, IReadConstantSource rcs)
        {
            var receiverReg = s.ReadInt();
            var valueReg = s.ReadInt();
            var fieldName = rcs.ReferenceStringConstant(s.ReadULong());
            var receiverClass = rcs.ReferenceClassConstant(s.ReadULong());
            return new WriteFieldInstruction(valueReg, receiverReg, fieldName, receiverClass);
        }
    }
}
