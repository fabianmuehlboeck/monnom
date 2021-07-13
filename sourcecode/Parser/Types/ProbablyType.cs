using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class ProbablyType : AType
    {
        public IType Type
        {
            get;
            private set;
        }

        public override string ReferenceName => Type.ReferenceName + "!";

        public ProbablyType(IType type, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Type = type;
        }

        public override T Visit<T>(TypeVisitor<T> visitor)
        {
            return visitor.ProbablyAction(this);
        }


        public override R Visit<S, R>(ITypeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitProbablyType(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Type.PrettyPrint(p);
            p.WritePunctuation("!");
        }
    }

    public partial interface ITypeVisitor<in S, out R>
    {
        Func<ProbablyType, S, R> VisitProbablyType { get; }
    }
}
