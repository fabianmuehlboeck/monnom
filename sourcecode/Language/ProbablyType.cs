using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public class ProbablyType : MaybeType
    {
        public ProbablyType(IType potentialType) : base(potentialType)
        {
        }

        public override string ReferenceName => PotentialType.ReferenceName+"!";

        public override bool IsSubtypeOf(IType other, bool optimistic = false)
        {
            if(optimistic)
            {
                return PotentialType.IsSubtypeOf(other);
            }
            return base.IsSubtypeOf(other, optimistic);
        }

        public override Ret Visit<Arg, Ret>(ITypeVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitProbablyType(this,arg);
        }
    }
}
