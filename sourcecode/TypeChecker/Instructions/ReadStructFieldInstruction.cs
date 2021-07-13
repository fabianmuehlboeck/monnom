using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class ReadStructFieldInstruction : AValueInstruction
    {
        public TDStructField Field { get; }
        public ReadStructFieldInstruction(TDStructField field, IRegister register) : base(register)
        {
            Field = field;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitReadStructFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReadStructFieldInstruction, Arg, Ret> VisitReadStructFieldInstruction { get; }
    }
}
