using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class RefIdentExpr : AExpr, IAccessorExpr
    {
        public RefIdentExpr(IExpr accessor, RefIdentifier identifier) : base(accessor.Start.SpanTo(identifier.End))
        {
            Accessor = accessor;
            Identifier = identifier;
        }
        public IExpr Accessor { get; }
        public RefIdentifier Identifier { get; }
        public override IEnumerable<Identifier> FreeVars => Accessor.FreeVars;

        public override void PrettyPrint(PrettyPrinter p)
        {
            if(Accessor!=null)
            {
                Accessor.PrettyPrint(p);
                p.WritePunctuation(".");
            }
            Identifier.PrettyPrint(p);
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitRefIdentExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<RefIdentExpr, S, R> VisitRefIdentExpr { get; }
    }
}
