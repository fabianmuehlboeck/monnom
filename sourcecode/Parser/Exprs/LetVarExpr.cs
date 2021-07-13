using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class LetVarExpr : AExpr
    {
        public readonly Identifier Ident;
        public readonly IExpr BindExpr;
        public readonly IExpr Body;
        public LetVarExpr(Identifier ident, IExpr bindExpr, IExpr body, ISourceSpan locs) : base(locs)
        {
            Ident = ident;
            BindExpr = bindExpr;
            Body = body;
        }

        public override IEnumerable<Identifier> FreeVars => BindExpr.FreeVars.Concat(Body.FreeVars);

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.WriteKeyword("letvar");
            p.WriteWhitespace();
            Ident.PrettyPrint(p);
            p.WriteWhitespace();
            p.WritePunctuation("=");
            p.WriteWhitespace();
            BindExpr.PrettyPrint(p);
            p.WriteWhitespace();
            p.WriteKeyword("in");
            p.WriteWhitespace();
            Body.PrettyPrint(p);
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitLetVarExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<LetVarExpr, S, R> VisitLetVarExpr { get; }
    }
}
