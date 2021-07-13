using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class DebugInstruction : AInstruction
    {
        public string Message { get; }

        public DebugInstruction(String message)
        {
            Message = message;
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
            return visitor.VisitDebugInstruction(this, arg);
        }
    }
    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<DebugInstruction, Arg, Ret> VisitDebugInstruction { get; }
    }
}
