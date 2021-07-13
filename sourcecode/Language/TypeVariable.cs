using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class TypeVariable : AType
    {
        public TypeVariable(ITypeParameterSpec paramSpec)
        {
            this.ParameterSpec = paramSpec;
        }

        public ITypeParameterSpec ParameterSpec
        {
            get;
        }

        public override string ReferenceName => ParameterSpec.Name;

        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
            }
        }

        public override bool IsDisjoint(IType other)
        {
            return ParameterSpec.UpperBound.IsDisjoint(other);
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            TypeVisitor<object, bool> tv = new TypeVisitor<object, bool>();
            tv.DefaultAction = (t, o) => false;
            tv.VisitDynamicType = (dt, o) => true;
            tv.VisitTypeVariable = (tvar, o) => tvar.ParameterSpec.Equals(this.ParameterSpec);
            tv.VisitMaybeType = (mb, o) => mb.IsSupertypeOf(this, optimistic);
            tv.VisitProbablyType = (pb, o) => pb.IsSupertypeOf(this, optimistic);
            return other.Visit(tv);
        }

        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            TypeVisitor<object, bool> tv = new TypeVisitor<object, bool>();
            tv.DefaultAction = (t, o) => ParameterSpec.LowerBound.IsSupertypeOf(t, optimistic);
            tv.VisitTypeVariable = (tvar, o) => tvar.ParameterSpec.Equals(this.ParameterSpec);
            tv.VisitMaybeType = (mb, o) => mb.IsSubtypeOf(this, optimistic);
            tv.VisitProbablyType = (pb, o) => pb.IsSubtypeOf(this, optimistic);
            return other.Visit(tv);
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            if(param.Equals(ParameterSpec))
            {
                return type;
            }
            return this;
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitTypeVariable(this, arg);
        }

        public override IType Join(IType other)
        {
            throw new NotImplementedException();
        }

        public override IType Meet(IType other)
        {
            throw new NotImplementedException();
        }

        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            if(env.Contains(ParameterSpec))
            {
                return env.GetType(ParameterSpec);
            }
            return this;
        }

        public override bool PrecisionRelated(IType other)
        {
            return other.Visit(new TypeVisitor<object,bool>()
            {
                DefaultAction=(x,o)=>false,
                VisitTypeVariable=(v,o)=>v.ParameterSpec==this.ParameterSpec,
                VisitDynamicType = (d, o) => true
            });
        }

        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitTypeVariable = (v, o) => v.ParameterSpec == this.ParameterSpec,
                VisitDynamicType = (d,o)=>true
            });
        }
    }
}
