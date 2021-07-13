using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ErrorStmt:AStmt
    {
        public IExpr Expr;
        public ErrorStmt(IExpr expr, ISourceSpan locs):base(locs)
        {
            this.Expr = expr;
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Expr.FreeVars;
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.WritePunctuation("ERROR(");
            Expr.PrettyPrint(p);
            p.WritePunctuation(");");
            p.WriteWhitespace();
        }


        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitErrorStmt(this, arg);
        }


    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<ErrorStmt, S, R> VisitErrorStmt { get; }
    }
}
