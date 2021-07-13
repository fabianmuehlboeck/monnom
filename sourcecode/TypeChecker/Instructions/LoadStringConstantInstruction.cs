using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class LoadStringConstantInstruction : AValueInstruction
    {
        public String Value { get; }
        public LoadStringConstantInstruction(String value, IRegister register) : base(register)
        {
            Value = value;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitLoadStringConstantInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<LoadStringConstantInstruction, Arg, Ret> VisitLoadStringConstantInstruction { get; }
    }
}
