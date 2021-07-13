using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class LetExpr : AExpr
    {
        public readonly StructFieldDecl Declaration;
        public readonly IExpr Body;
        public LetExpr(StructFieldDecl sfd, IExpr body, ISourceSpan locs) : base(locs)
        {
            Declaration = sfd;
            Body = body;
        }

        public override IEnumerable<Identifier> FreeVars => Declaration.InitExpr.FreeVars.Concat(Body.FreeVars);

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.WriteKeyword("let");
            p.WriteWhitespace();
            Declaration.PrettyPrint(p);
            p.WriteWhitespace();
            p.WriteKeyword("in");
            p.WriteWhitespace();
            Body.PrettyPrint(p);
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitLetExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<LetExpr, S, R> VisitLetExpr { get; }
    }
}
