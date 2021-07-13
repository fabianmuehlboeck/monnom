using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class RuntimeCmdInstruction : AInstruction
    {
        public string Cmd { get; }

        public RuntimeCmdInstruction(String cmd)
        {
            Cmd = cmd;
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
            return visitor.VisitRuntimeCmdInstruction(this, arg);
        }
    }
    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<RuntimeCmdInstruction, Arg, Ret> VisitRuntimeCmdInstruction { get; }
    }
}
