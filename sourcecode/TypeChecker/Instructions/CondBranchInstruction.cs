using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.TypeChecker
{
    public class CondBranchInstruction : AInstruction, IBranchInstruction
    {
        private abstract class ASubBranch : IBranchInstruction
        {
            public readonly CondBranchInstruction Parent;
            public ASubBranch(CondBranchInstruction parent)
            {
                Parent = parent;
            }

            public BranchEnvironment OutEnvironment => Parent.OutEnvironment;

            public IEnumerable<IRegister> WriteRegisters => Parent.WriteRegisters;

            public abstract void RegisterIncoming(IRegister to, IRegister from);

            public Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
            {
                return Parent.Visit(visitor, arg);
            }
        }
        private class ThenBranch : ASubBranch
        {
            public ThenBranch(CondBranchInstruction parent) : base(parent)
            {
            }

            public override void RegisterIncoming(IRegister to, IRegister from)
            {
                Parent.thenIncomings.Add((to, from));
            }
        }
        private class ElseBranch : ASubBranch
        {
            public ElseBranch(CondBranchInstruction parent) : base(parent)
            {
            }

            public override void RegisterIncoming(IRegister to, IRegister from)
            {
                Parent.elseIncomings.Add((to, from));
            }
        }
        public readonly IRegister Condition;
        public readonly PhiNode ThenTarget;
        public readonly PhiNode ElseTarget;
        public CondBranchInstruction(IRegister cond, PhiNode thenTarget, PhiNode elseTarget, ICodeTransformEnvironment env)
        {
            Condition = cond;
            ThenTarget = thenTarget;
            ElseTarget = elseTarget;
            OutEnvironment = new BranchEnvironment(env);
            thenTarget.RegisterIncomingBranch(new ThenBranch(this));
            elseTarget.RegisterIncomingBranch(new ElseBranch(this));
        }

        private List<(IRegister, IRegister)> thenIncomings=new List<(IRegister, IRegister)>();
        public IEnumerable<(IRegister, IRegister)> GetThenIncomings()
        {
            return thenIncomings.ToList();
        }
        private List<(IRegister, IRegister)> elseIncomings = new List<(IRegister, IRegister)>();
        public IEnumerable<(IRegister, IRegister)> GetElseIncomings()
        {
            return elseIncomings.ToList();
        }

        public override IEnumerable<IRegister> WriteRegisters => thenIncomings.Select(p => p.Item1).Concat(elseIncomings.Select(p => p.Item1)).Distinct();

        public BranchEnvironment OutEnvironment { get; }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitCondBranchInstruction(this, arg);
        }

        public void RegisterIncoming(IRegister to, IRegister from)
        {
            throw new InternalException("Incoming registers need to be registered via then- and else-subobjects");
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CondBranchInstruction, Arg, Ret> VisitCondBranchInstruction { get; }
    }
}
