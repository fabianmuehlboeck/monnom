using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class NullExpr : ALiteralExpr, IExpr
    {
        public NullExpr(IToken tok=null):base(tok)
        {

        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("null", Start);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitNullExpr(this, state);
        }
    }

    public class DefaultNullExpr : NullExpr, IDefaultExpr
    {
        public DefaultNullExpr(IToken token)
            : base(token)
        {

        }

        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultNullExpr(this, state);
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultNullExpr(this, state);
        }
    }
    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultNullExpr, S, R> VisitDefaultNullExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<NullExpr, S, R> VisitNullExpr { get; }
    }
}
