using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class BoolTrueInstruction : AValueInstruction
    {
        public BoolTrueInstruction(IRegister register) : base(register)
        {
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBoolTrueInstruction(this, arg);
        }
    }
    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<BoolTrueInstruction, Arg, Ret> VisitBoolTrueInstruction { get; }
    }
}
