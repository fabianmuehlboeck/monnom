using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class ErrorInstruction : AInstruction
    {
        public IRegister Register { get; }
        public ErrorInstruction(IRegister reg)
        {
            Register = reg;
        }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitErrorInstruction(this, arg);
        }
    }
    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ErrorInstruction, Arg, Ret> VisitErrorInstruction { get; }
    }
}
