using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class ReturnInstruction : AInstruction
    {
        public IRegister Register { get; }
        public ReturnInstruction(IRegister register)
        {
            Register = register;
        }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReturnInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReturnInstruction, Arg, Ret> VisitReturnInstruction { get; }
    }
}
