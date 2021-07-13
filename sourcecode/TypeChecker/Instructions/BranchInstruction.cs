using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.TypeChecker
{
    public class BranchInstruction : AInstruction, IBranchInstruction
    {
        public readonly PhiNode Target;
        public BranchInstruction(PhiNode phi, ICodeTransformEnvironment env)
        {
            Target = phi;
            OutEnvironment = new BranchEnvironment(env);
            phi.RegisterIncomingBranch(this);
        }

        public IEnumerable<(IRegister, IRegister)> GetIncomings()
        {
            return incomings.ToList();
        }

        private List<(IRegister, IRegister)> incomings = new List<(IRegister, IRegister)>();
        public override IEnumerable<IRegister> WriteRegisters => incomings.Select(p => p.Item1);
        public BranchEnvironment OutEnvironment { get; }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBranchInstruction(this, arg);
        }

        public void RegisterIncoming(IRegister to, IRegister from)
        {
            incomings.Add((to, from));
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<BranchInstruction, Arg, Ret> VisitBranchInstruction { get; }
    }
}
