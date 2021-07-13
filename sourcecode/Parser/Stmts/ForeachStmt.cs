using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class ForeachStmt : AStmt
    {
        public readonly IExpr Range;
        public readonly VarDecl Var;
        public readonly Block Block;
        public ForeachStmt(VarDecl var, IExpr range, Block block, ISourceSpan locs):base(locs)
        {
            this.Var = var;
            this.Range = range;
            this.Block = block;
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Range.FreeVars.Concat(Block.Select(s => s.UsedIdentifiers).Flatten());
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("foreach", this.Start);
            p.WritePunctuation("(");
            Var.PrettyPrint(p);
            p.WriteKeyword("in");
            p.IncreaseIndent();
            Range.PrettyPrint(p);
            p.WritePunctuation(")");
            p.DecreaseIndent();
            Block.PrettyPrint(p);
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitForeachStmt(this, arg);
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<ForeachStmt, S, R> VisitForeachStmt { get; }
    }
}
