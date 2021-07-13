using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class TopType : AType
    {
        public static readonly TopType Instance = new TopType();

        public override string ReferenceName => "Anything";

        public override IEnumerable<IType> InheritsFrom
        {
            get
            {
                yield return this;
            }
        }

        private TopType()
        {

        }

        public override bool IsDisjoint(IType other)
        {
            return other.IsSubtypeOf(BotType.Instance, false);
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            if (other.Equals(BotType.Instance))
            {
                return false;
            }
            return other.IsSupertypeOf(this, optimistic);
        }
        
        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            return true;
        }

        public override IType Join(IType other)
        {
            return this;
        }

        public override IType Meet(IType other)
        {
            return other;
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            return this;
        }
        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            return this;
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitTopType(this, arg);
        }


        public override bool PrecisionRelated(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitTopType = (v, o) => true,
                VisitDynamicType = (d, o) => true
            });
        }

        public override bool LessOptimistic(IType other)
        {
            return other.Visit(new TypeVisitor<object, bool>()
            {
                DefaultAction = (x, o) => false,
                VisitTopType = (v, o) => true,
                VisitDynamicType = (d, o) => true
            });
        }
    }
}
