using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class MaybeType : AType
    {
        public readonly IType PotentialType;
        public MaybeType(IType potentialType)
        {
            PotentialType = potentialType;
        }

        public override string ReferenceName => PotentialType.ReferenceName + "?";

        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
            }
        }

        public override bool IsDisjoint(IType other)
        {
            return other.IsDisjoint(PotentialType) && other.IsDisjoint(ClassType.NullType);
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            return PotentialType.IsSubtypeOf(other, optimistic) && ClassType.NullType.IsSubtypeOf(other, optimistic);
        }

        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            return PotentialType.IsSupertypeOf(other, optimistic) || ClassType.NullType.IsSupertypeOf(other, optimistic);
        }

        public override IType Join(IType other)
        {
            return new MaybeType(PotentialType.Join(other));
        }

        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitMaybeType = (m, o) => PotentialType.LessOptimistic(m.PotentialType)
            }) ;
        }

        public override IType Meet(IType other)
        {
            throw new NotImplementedException();
        }

        public override bool PrecisionRelated(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitDynamicType = (d, o) => true,
                VisitMaybeType = (m, o) => PotentialType.PrecisionRelated(m.PotentialType)
            });
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitMaybeType(this, arg);
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            return new MaybeType(((ISubstitutable<IType>)PotentialType).Substitute(param, type));
        }

        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            return new MaybeType(((ISubstitutable<IType>)PotentialType).Substitute(env));
        }
    }
}
