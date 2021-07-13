using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class LoadIntConstantInstruction : AValueInstruction
    {
        public LoadIntConstantInstruction(Int64 value, IRegister register):base(register)
        {
            Value = value;
        }
        public Int64 Value { get; }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitLoadIntConstantInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<LoadIntConstantInstruction, Arg, Ret> VisitLoadIntConstantInstruction { get; }
    }
}
