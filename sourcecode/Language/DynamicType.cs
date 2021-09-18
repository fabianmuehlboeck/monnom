using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class DynamicType : AType
    {
        public override string ReferenceName => "dynamic";
        
        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
            }
        }

        public override bool IsDisjoint(IType other)
        {
            return other.IsSubtypeOf(BotType.Instance, false);
        }

        public override bool IsEquivalent(IType other, bool optimistic = false)
        {
            var visitor = new TypeVisitor<object, bool>();
            visitor.DefaultAction = (t, o) => optimistic;
            visitor.VisitDynamicType = (t, o) => true;
            return other.Visit(visitor, null);
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            return optimistic || other.IsSupertypeOf(TopType.Instance,false);
        }

        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            return true;
        }

        public override IType Join(IType other)
        {
            return this;
        }



        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x,o)=>false,
                VisitDynamicType = (d,o)=>true
            });
        }

        public override IType Meet(IType other)
        {
            return other;
        }


        public override bool PrecisionRelated(IType other)
        {
            return true;
        }

        public override IType ReplaceArgsWith(IEnumerable<IType> args)
        {
            throw new InvalidOperationException();
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitDynamicType(this, arg);
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            return this;
        }

        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            return this;
        }

    }

    public partial interface ITypeVisitor<in Arg, out Ret>
    {
        Func<DynamicType, Arg, Ret> VisitDynamicType { get; }
    }
}
