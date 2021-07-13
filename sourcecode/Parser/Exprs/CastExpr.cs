using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class CastExpr : AExpr
    {
        public readonly IType Type;
        public readonly IExpr Expr;
        public CastExpr(IType t, IExpr e, ISourceSpan locs):base(locs)
        {
            this.Type = t;
            this.Expr = e;
        }

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Expr.FreeVars;
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("(", Start);
            Type.PrettyPrint(p);
            p.WritePunctuation(")");
            Expr.PrettyPrint(p);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitCastExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<CastExpr, S, R> VisitCastExpr { get; }
    }
}
