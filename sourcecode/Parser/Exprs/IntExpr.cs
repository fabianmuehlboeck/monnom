using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class IntExpr : ALiteralExpr, IExpr
    {
        public IntExpr(IToken token, bool negative=false)
            : base(token,negative?"-":"")
        {

        }

        public IntExpr(int value, ISourceSpan locs=null)
            : base(value.ToString(), locs??new GenSourceSpan())
        {
            this.Value = value.ToString();
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write(Value, Start);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitIntExpr(this, state);
        }
    }

    public class DefaultIntExpr : IntExpr, IDefaultExpr
    {
        public DefaultIntExpr(IToken token, bool negative = false)
            : base(token, negative)
        {

        }

        public DefaultIntExpr(int value, ISourceSpan locs = null)
            : base(value, locs)
        {
        }
        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultIntExpr(this, state);
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultIntExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultIntExpr, S, R> VisitDefaultIntExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<IntExpr, S, R> VisitIntExpr { get; }
    }
}
