using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class BinOpExpr : AOpExpr
    {
        public BinaryOperatorNode Operator
        {
            get;
            private set;
        }
        public BinOpExpr(IExpr left, BinaryOperatorNode op, IExpr right, ISourceSpan locs = null) : base(left, right, locs)
        {
            this.Operator = op;
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitBinOpExpr(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {

            p.WriteKeyword("(");
            Left.PrettyPrint(p);
            p.WriteWhitespace();
            Operator.PrettyPrint(p);
            p.WriteWhitespace();
            Right.PrettyPrint(p);
            p.WriteKeyword(")");
            p.WriteWhitespace();

        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<BinOpExpr, S, R> VisitBinOpExpr { get; }
    }
}
