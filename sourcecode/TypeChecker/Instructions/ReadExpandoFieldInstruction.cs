using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class ReadExpandoFieldInstruction : AValueInstruction
    {
        public String FieldName { get; }
        public IRegister Receiver { get; }
        public ReadExpandoFieldInstruction(String fieldName, IRegister receiver, IRegister register) : base(register)
        {
            FieldName = fieldName;
            Receiver = receiver;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReadExpandoFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReadExpandoFieldInstruction, Arg, Ret> VisitReadExpandoFieldInstruction { get; }
    }
}
