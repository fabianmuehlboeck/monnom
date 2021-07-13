using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class WriteExpandoFieldInstruction : AInstruction
    {
        public String FieldName { get; }
        public IRegister Receiver { get; }
        public IRegister Value { get; }
        public WriteExpandoFieldInstruction(String fieldName, IRegister receiver, IRegister value)
        {
            FieldName = fieldName;
            Receiver = receiver;
            Value = value;
        }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitWriteExpandoFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<WriteExpandoFieldInstruction, Arg, Ret> VisitWriteExpandoFieldInstruction { get; }
    }
}
