using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ExprStmt : AStmt
    {
        private IExpr expr;
        public IExpr Expression => expr;
        public ExprStmt(IExpr e, ISourceSpan locs):base(locs)
        {
            this.expr = e;
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return expr.FreeVars;
            }
        }
        
        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitExprStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Expression.PrettyPrint(p);
            p.WritePunctuation(";");
            p.WriteWhitespace();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<ExprStmt, S, R> VisitExprStmt { get; }
    }
}
