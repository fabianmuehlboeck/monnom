using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ReturnStmt : AStmt
    {
        public readonly IExpr Expr;
        public ReturnStmt(IExpr e, ISourceSpan locs):base(locs)
        {
            this.Expr = e;
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
            p.Write("return", Start);
            p.WriteWhitespace();
            Expr.PrettyPrint(p);
            p.WritePunctuation(";");
            p.WriteWhitespace();
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReturnStmt(this, arg);
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<ReturnStmt, S, R> VisitReturnStmt { get; }
    }
}
