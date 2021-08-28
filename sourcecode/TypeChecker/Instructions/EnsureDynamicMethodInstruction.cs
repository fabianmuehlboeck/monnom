using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class EnsureDynamicMethodInstruction : AInstruction
    {
        public IRegister Receiver { get; }
        public string MethodName { get; }
        public EnsureDynamicMethodInstruction(IRegister receiver, string methodName)
        {
            Receiver = receiver;
            MethodName = methodName;
        }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitEnsureDynamicMethodInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<EnsureDynamicMethodInstruction, Arg, Ret> VisitEnsureDynamicMethodInstruction { get; }
    }
}
