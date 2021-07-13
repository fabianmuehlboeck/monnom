using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class ReturnVoidInstruction : AInstruction
    {
        public ReturnVoidInstruction()
        {
        }

        public override IEnumerable<IRegister> WriteRegisters
        {
            get
            {
                yield break;
            }
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReturnVoidInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReturnVoidInstruction, Arg, Ret> VisitReturnVoidInstruction { get; }
    }
}
