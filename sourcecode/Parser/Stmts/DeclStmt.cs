using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class DeclStmt : AStmt
    {
        private readonly Identifier var;

        public IType Type
        {
            get;
        }
        public DeclStmt(IType type, Identifier var, IExpr expr, ISourceSpan locs) : base(locs)
        {
            this.var = var;
            this.Expr = expr;
            this.Type = type;
        }

        public Identifier Var
        {
            get
            {
                return var;
            }
        }
        public IExpr Expr { get; }


        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitDeclStmt(this, arg);
        }


        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Expr.FreeVars.Cons(var);
            }
        }
        public override void PrettyPrint(PrettyPrinter p)
        {
            Type?.PrettyPrint(p);
            p.WriteWhitespace();
            Var.PrettyPrint(p);
            if (Expr != null)
            {
                p.WriteKeyword("=");
                Expr.PrettyPrint(p);
            }
            p.WritePunctuation(";");
            p.WriteWhitespace();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<DeclStmt, S, R> VisitDeclStmt { get; }
    }
}
