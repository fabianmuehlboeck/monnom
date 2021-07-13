using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class FloatExpr : ALiteralExpr, IExpr
    {

        public FloatExpr(IToken token, bool negative = false)
            : base(token, negative ? "-" : "")
        {

        }

        public FloatExpr(double value, ISourceSpan locs)
            : base(value.ToString(), locs??new GenSourceSpan())
        {
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write(Value, Start);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitFloatExpr(this, state);
        }
    }

    public class DefaultFloatExpr : FloatExpr, IDefaultExpr
    {
        public DefaultFloatExpr(IToken token, bool negative = false)
            : base(token, negative)
        {

        }

        public DefaultFloatExpr(int value, ISourceSpan locs = null)
            : base(value, locs)
        {
        }
        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultFloatExpr(this, state);
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultFloatExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultFloatExpr, S, R> VisitDefaultFloatExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<FloatExpr, S, R> VisitFloatExpr { get; }
    }
}
