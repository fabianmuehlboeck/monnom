using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class BoolFalseInstruction : AValueInstruction
    {
        public BoolFalseInstruction(IRegister register) : base(register)
        {
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBoolFalseInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<BoolFalseInstruction, Arg, Ret> VisitBoolFalseInstruction { get; }
    }
}
