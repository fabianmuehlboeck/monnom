using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.Parser;

namespace Nom.TypeChecker
{
    public abstract class PhiNode : AScopedChildTransformEnvironment, IInstruction
    {
        public abstract IEnumerable<(IRegister, Language.IType)> GetRegisters();
        public abstract void FinalizeMerges();

        public int Index { get; set; }
        public abstract int IncomingCount { get; }

        public PhiNode() : base(null)
        {
        }

        public IEnumerable<IRegister> WriteRegisters => GetRegisters().Select(p => p.Item1);



        public Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitPhiNode(this, arg);
        }

        public abstract void RegisterIncomingBranch(IBranchInstruction branch);

    }

    public class LoopPhiNode : PhiNode
    {
        protected class LoopVariableCopy : ALocalVariableCopy
        {
            public class WrapRegister : IRegister
            {
                public IRegister Parent { get; set; }
                public WrapRegister(IRegister parent)
                {
                    Parent = parent;
                }

                public int Index => Parent.Index;
            }
            private List<ILocalVariable> mergeSources = new List<ILocalVariable>();
            public LoopVariableCopy(ILocalVariable parent) : base(parent)
            {
                if (parent.CurrentRegister != null)
                {
                    reg = new WrapRegister(parent.CurrentRegister);
                }
                else
                {
                    reg = null;
                }
                CurrentRegister = reg;
            }
            private WrapRegister reg;

            public override void UpdateFrom(ILocalVariable other)
            {
                CurrentRegister = other.CurrentRegister;
            }

            public IOptional<(IRegister, Language.IType)> FinalizeMerges(IEnumerable<IBranchInstruction> sources, LoopPhiNode parent)
            {
                var dict = sources.ToDictionary<IBranchInstruction, IBranchInstruction, IRegister>(x => x, x => x.OutEnvironment.IsAncestor(parent) ? x.OutEnvironment.GetTopmostVersion(this).CurrentRegister : x.OutEnvironment.GetCurrentVersion(this).CurrentRegister);
                if (dict.Values.Any(r => r?.Index != reg?.Parent.Index) && !dict.Values.Any(r => r == null))
                {
                    reg.Parent = parent.CreateRegister();
                    foreach (IBranchInstruction source in sources)
                    {
                        source.RegisterIncoming(reg.Parent, dict[source]);
                    }
                    return (reg.Parent, Type).InjectOptional();
                }
                return Optional<(IRegister, Language.IType)>.Empty;
            }
        }
        public LoopPhiNode(ICodeTransformEnvironment parent) : base()
        {
            Parent = parent;
        }

        private Dictionary<string, LoopVariableCopy> loopVars = new Dictionary<string, LoopVariableCopy>();

        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            loopVars.Add(lv.Name, new LoopVariableCopy(lv));
            loopVars[lv.Name].RegisterChange += RaiseVariableRegisterChanged;
            return loopVars[lv.Name];
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            return loopVars.GetOptional(name);
        }

        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => loopVars.Values;

        public override int IncomingCount => incomingBranches.Count;

        private List<IBranchInstruction> incomingBranches = new List<IBranchInstruction>();

        public override void RegisterIncomingBranch(IBranchInstruction branch)
        {
            incomingBranches.Add(branch);
        }

        private List<(IRegister, Language.IType)> registers;
        public override IEnumerable<(IRegister, Language.IType)> GetRegisters()
        {
            return registers.ToList();
        }

        public override void FinalizeMerges()
        {
            if (registers != null)
            {
                throw new InternalException("Cannot finalize phi node twice!");
            }
            registers = new List<(IRegister, Language.IType)>();
            foreach (LoopVariableCopy lvc in loopVars.Values)
            {
                lvc.FinalizeMerges(incomingBranches, this).ActionBind(regp => registers.Add(regp));
            }
        }

        private PhiNode breakPHI;
        private PhiNode continuePHI;
        public void RegisterTargets(PhiNode breakTarget, PhiNode continueTarget)
        {
            breakPHI = breakTarget;
            continuePHI = continueTarget;
        }

        public override PhiNode GetBreakPHI(int levels)
        {
            if (levels <= 0)
            {
                return breakPHI;
            }
            return Parent.GetBreakPHI(levels - 1);
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            if (levels <= 0)
            {
                return continuePHI;
            }
            return Parent.GetContinuePHI(levels - 1);
        }
    }

    public class LoopOutPhiNode : PhiNode
    {
        private class LoopEnvironment : AScopedChildTransformEnvironment
        {
            public class LoopVariableCopy : ALocalVariableCopy
            {
                private List<ILocalVariable> mergeSources = new List<ILocalVariable>();
                public LoopVariableCopy(ILocalVariable parent) : base(parent)
                {
                    CurrentRegister = parent.CurrentRegister;
                }

                public override void UpdateFrom(ILocalVariable other)
                {
                    CurrentRegister = other.CurrentRegister;
                }
                public IOptional<(IRegister, Language.IType)> FinalizeMerges(IEnumerable<IBranchInstruction> sources, LoopOutPhiNode parent)
                {
                    var dict = sources.ToDictionary<IBranchInstruction, IBranchInstruction, IRegister>(x => x, x => x.OutEnvironment.GetCurrentVersion(this).CurrentRegister);
                    if (dict.Values.Any(r => r?.Index != CurrentRegister?.Index) && !dict.Values.Any(r => r == null))
                    {
                        IRegister newReg = parent.CreateRegister();
                        foreach (IBranchInstruction source in sources)
                        {
                            source.RegisterIncoming(newReg, dict[source]);
                        }
                        return (newReg, Type).InjectOptional();
                    }
                    return Optional<(IRegister, Language.IType)>.Empty;
                }

            }
            private ICodeTransformEnvironment ParentEnv { get; }

            private Dictionary<string, LoopVariableCopy> variableCopies = new Dictionary<string, LoopVariableCopy>();

            public LoopEnvironment(ICodeTransformEnvironment parent) : base(parent)
            {
            }

            protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => variableCopies.Values;
            public IEnumerable<LoopVariableCopy> VariableCopies => variableCopies.Values;

            public override PhiNode GetBreakPHI(int levels)
            {
                return ParentEnv.GetBreakPHI(levels);
            }

            public override PhiNode GetContinuePHI(int levels)
            {
                return ParentEnv.GetBreakPHI(levels);
            }

            protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
            {
                var lvc= new LoopVariableCopy(lv);
                this.variableCopies.Add(lv.Name, lvc);
                lvc.RegisterChange += RaiseVariableRegisterChanged;
                return lvc;
            }

            protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
            {
                return variableCopies.GetOptional(name);
            }

        }
        private LoopEnvironment loopEnv;
        public ICodeTransformEnvironment LoopEnv => loopEnv;
        public LoopOutPhiNode(ICodeTransformEnvironment parent) : base()
        {
            Parent = parent;
            loopEnv = new LoopEnvironment(parent);
        }

        protected class LoopOutVariableCopy : ALocalVariableCopy
        {
            public LoopOutVariableCopy(ILocalVariable parent) : base(parent)
            {
            }


            public override void UpdateFrom(ILocalVariable other)
            {
                CurrentRegister = other.CurrentRegister;
            }
        }

        private Dictionary<string, LoopOutVariableCopy> loopVars = new Dictionary<string, LoopOutVariableCopy>();

        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            loopVars.Add(lv.Name, new LoopOutVariableCopy(lv));
            loopVars[lv.Name].RegisterChange += RaiseVariableRegisterChanged;
            return loopVars[lv.Name];
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            return loopVars.GetOptional(name);
        }

        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => loopVars.Values;

        public override int IncomingCount => incomingBranches.Count;

        private List<IBranchInstruction> incomingBranches = new List<IBranchInstruction>();

        public override void RegisterIncomingBranch(IBranchInstruction branch)
        {
            incomingBranches.Add(branch);
        }

        private List<(IRegister, Language.IType)> registers;
        public override IEnumerable<(IRegister, Language.IType)> GetRegisters()
        {
            return registers.ToList();
        }

        public override void FinalizeMerges()
        {
            if (registers != null)
            {
                throw new InternalException("Cannot finalize phi node twice!");
            }
            registers = new List<(IRegister, Language.IType)>();
            foreach (LoopEnvironment.LoopVariableCopy lvc in loopEnv.VariableCopies)
            {
                lvc.FinalizeMerges(incomingBranches, this).ActionBind(regp =>
                {
                    var lovc = new LoopOutVariableCopy(lvc.Parent);
                    lovc.CurrentRegister = regp.Item1;
                    loopVars.Add(lvc.Name, lovc);
                    registers.Add(regp);
                });
            }
        }

        public override PhiNode GetBreakPHI(int levels)
        {
            return Parent.GetBreakPHI(levels - 1);
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            return Parent.GetContinuePHI(levels - 1);
        }
    }

    public class MergePhiNode : PhiNode
    {
        protected class MergeVariableCopy : ALocalVariableCopy
        {
            public MergeVariableCopy(ILocalVariable parent) : base(parent)
            {
                reg = parent.CurrentRegister;
                CurrentRegister = reg;
            }

            private IRegister reg;

            public override void UpdateFrom(ILocalVariable other)
            {
                throw new InternalException("Merge-Variable-Copies should not be updated through a merge!");
            }
            public IOptional<(IRegister, Language.IType)> FinalizeMerges(IEnumerable<IBranchInstruction> sources, MergePhiNode parent)
            {
                var dict = sources.ToDictionary<IBranchInstruction, IBranchInstruction, IRegister>(x => x, x => x.OutEnvironment.GetCurrentVersion(this).CurrentRegister);
                if (dict.Values.Any(r => r?.Index != reg?.Index) && !dict.Values.Any(r => r == null))
                {
                    reg = parent.CreateRegister();
                    foreach (IBranchInstruction source in sources)
                    {
                        source.RegisterIncoming(reg, dict[source]);
                    }
                    CurrentRegister = reg;
                    return (reg, Type).InjectOptional();
                }
                return Optional<(IRegister, Language.IType)>.Empty;
            }
        }

        public MergePhiNode(ICodeTransformEnvironment parent) : base()
        {
            Parent = parent;
        }


        private Dictionary<string, MergeVariableCopy> mergeVars = new Dictionary<string, MergeVariableCopy>();

        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            mergeVars.Add(lv.Name, new MergeVariableCopy(lv));
            mergeVars[lv.Name].RegisterChange += RaiseVariableRegisterChanged;
            return mergeVars[lv.Name];
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            return mergeVars.GetOptional(name);
        }

        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => mergeVars.Values;

        public override void MergeIn(ICodeTransformEnvironment env)
        {
            throw new InternalException("Cannot merge into phi nodes!");
        }

        private List<(IRegister, Language.IType)> registers;
        public override IEnumerable<(IRegister, Language.IType)> GetRegisters()
        {
            return registers.ToList();
        }

        public override void FinalizeMerges()
        {
            if (registers != null)
            {
                throw new InternalException("Cannot finalize phi node twice!");
            }
            registers = new List<(IRegister, Language.IType)>();
            foreach (MergeVariableCopy lvc in mergeVars.Values)
            {
                lvc.FinalizeMerges(incomingBranches, this).ActionBind(regp => registers.Add(regp));
            }
        }

        private List<IBranchInstruction> incomingBranches = new List<IBranchInstruction>();
        public override int IncomingCount => incomingBranches.Count;

        public override void RegisterIncomingBranch(IBranchInstruction branch)
        {
            incomingBranches.Add(branch);
        }
        public override PhiNode GetBreakPHI(int levels)
        {
            return Parent.GetBreakPHI(levels);
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            return Parent.GetContinuePHI(levels);
        }
    }

    public class TargetPhiNode : PhiNode
    {
        protected class LocalVariableCopy : ALocalVariableCopy
        {

            public LocalVariableCopy(ILocalVariable parent) : base(parent)
            {
                CurrentRegister = parent.CurrentRegister;
            }


            public override void UpdateFrom(ILocalVariable other)
            {
                CurrentRegister = other.CurrentRegister;
            }
        }
        private Dictionary<string, LocalVariableCopy> localVariableCopies = new Dictionary<string, LocalVariableCopy>();
        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => localVariableCopies.Values;
        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            var lvc = new LocalVariableCopy(lv);
            localVariableCopies.Add(lv.Name, lvc);
            lvc.RegisterChange += RaiseVariableRegisterChanged;
            return lvc;
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            if (localVariableCopies.ContainsKey(name))
            {
                return localVariableCopies[name].InjectOptional();
            }
            return Optional<ALocalVariableCopy>.Empty;
        }
        public TargetPhiNode() : base()
        {
        }

        public override IEnumerable<(IRegister, Language.IType)> GetRegisters()
        {
            yield break;
        }

        public override void RegisterIncomingBranch(IBranchInstruction branch)
        {
            if (Parent != null)
            {
                throw new InternalException("Cannot have two branches targeting TargetPhiNode");
            }
            Parent = branch.OutEnvironment.GetParent();
        }

        public override void FinalizeMerges()
        {
        }

        public override PhiNode GetBreakPHI(int levels)
        {
            return Parent.GetBreakPHI(levels);
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            return Parent.GetContinuePHI(levels);
        }

        public override int IncomingCount => Parent == null ? 0 : 1;
    }

    public class BranchEnvironment : AScopedChildTransformEnvironment
    {
        private class LocalVariableCopy : ALocalVariableCopy
        {
            public LocalVariableCopy(ILocalVariable parent) : base(parent)
            {
                CurrentRegister = parent.CurrentRegister;
            }

            public override void UpdateFrom(ILocalVariable other)
            {
                throw new InternalException("Branch environment copy should not be used for code generation");
            }
        }
        public ICodeTransformEnvironment GetParent()
        {
            return Parent;
        }
        public BranchEnvironment(ICodeTransformEnvironment parent) : base(parent)
        {
            parent.VariableRegisterChanged += Parent_VariableRegisterChanged;
        }

        private void Parent_VariableRegisterChanged(ICodeTransformEnvironment arg1, ILocalVariable arg2, IRegister arg3, IRegister arg4)
        {
            if(!varCopies.ContainsKey(arg2.Name))
            {
                var x = this[new Identifier(arg2.Name)];
            }
        }

        private Dictionary<string, LocalVariableCopy> varCopies = new Dictionary<string, LocalVariableCopy>();
        protected override IEnumerable<ALocalVariableCopy> LocalVariableCopies => varCopies.Values;

        public override PhiNode GetBreakPHI(int levels)
        {
            throw new InternalException("Branch environment copy should not be used for code generation");
        }

        public override PhiNode GetContinuePHI(int levels)
        {
            throw new InternalException("Branch environment copy should not be used for code generation");
        }

        protected override ALocalVariableCopy CreateLocalVariableCopy(ILocalVariable lv)
        {
            var lvc = new LocalVariableCopy(lv);
            varCopies.Add(lv.Name, lvc);
            lvc.RegisterChange += (x,y,z)=> throw new InternalException("Branch environment copy should not be used for code generation");
            return lvc;
        }

        protected override IOptional<ALocalVariableCopy> GetLocalVariableCopy(string name)
        {
            return varCopies.GetOptional(name);
        }

        public bool IsAncestor(ICodeTransformEnvironment env)
        {
            return env.DescendedFrom(Parent);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<PhiNode, Arg, Ret> VisitPhiNode { get; }
    }
}
