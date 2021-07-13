using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class AssignStmt : AStmt
    {
        public readonly Identifier Variable;
        public readonly IExpr Expression;
        public readonly IOptional<IExpr> Accessor;

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Expression.FreeVars.Cons(Variable);
            }
        }

        public AssignStmt(IExpr accessor, Identifier var, IExpr expr, ISourceSpan locs = null) : base(locs ?? new GenSourceSpan())
        {
            this.Accessor = accessor.InjectOptional();
            this.Variable = var;
            this.Expression = expr;
        }
        

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitAssignStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Accessor.ActionBind(a => { a.PrettyPrint(p); p.WritePunctuation("."); });
            Variable.PrettyPrint(p);
            p.WriteKeyword("=");
            Expression.PrettyPrint(p);
            p.WritePunctuation(";");
            p.WriteWhitespace();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<AssignStmt, S, R> VisitAssignStmt { get; }
    }
}
