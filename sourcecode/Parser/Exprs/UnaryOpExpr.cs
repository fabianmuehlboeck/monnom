using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class UnaryOpExpr : AExpr
    {
        public IExpr Expr { get; }
        public UnaryOperator Operator { get; }

        public UnaryOpExpr(IExpr expr, UnaryOperator op, ISourceSpan locs) : base(locs)
        {
            Expr = expr;
            Operator = op;
        }

        public override IEnumerable<Identifier> FreeVars => Expr.FreeVars;


        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitUnaryOpExpr(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            switch (Operator)
            {
                case UnaryOperator.Not:
                    p.Write("!", Start);
                    break;
                case UnaryOperator.Negate:
                    p.Write("-", Start);
                    break;
            }
            Expr.PrettyPrint(p);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<UnaryOpExpr, S, R> VisitUnaryOpExpr { get; }
    }
}
