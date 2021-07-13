using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class BotType : AType
    {
        public static readonly BotType Instance = new BotType();

        public override string ReferenceName => "Nothing";

        public override IEnumerable<IType> InheritsFrom => throw new NotImplementedException();

        private BotType()
        {

        }

        public override bool IsDisjoint(IType other)
        {
            return true;
        }

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            return true;
        }

        public override bool IsSupertypeOf(IType other, bool optimistic = false)
        {
            return other.IsSubtypeOf(this, optimistic);
        }

        public override IType Join(IType other)
        {
            return other;
        }

        public override IType Meet(IType other)
        {
            return this;
        }

        protected override IType SubstituteType(ITypeParameterSpec param, IType type)
        {
            return this;
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBotType(this, arg);
        }

        protected override IType SubstituteType<T>(ITypeEnvironment<T> env)
        {
            return this;
        }

        public override bool PrecisionRelated(IType other)
        {
            throw new NotImplementedException();
        }

        public override bool LessOptimistic(IType other)
        {
            throw new NotImplementedException();
        }
    }
}
