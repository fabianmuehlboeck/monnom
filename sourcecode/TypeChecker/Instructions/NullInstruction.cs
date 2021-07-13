using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class NullInstruction : AValueInstruction
    {
        public NullInstruction(IRegister register) : base(register)
        {
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitNullInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<NullInstruction, Arg, Ret> VisitNullInstruction { get; }
    }
}
