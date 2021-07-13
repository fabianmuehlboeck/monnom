using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class LoadFloatConstantInstruction : AValueInstruction
    {
        public LoadFloatConstantInstruction(Double value, IRegister register) : base(register)
        {
            Value = value;
        }
        public Double Value { get; }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitLoadFloatConstantInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<LoadFloatConstantInstruction, Arg, Ret> VisitLoadFloatConstantInstruction { get; }
    }
}
