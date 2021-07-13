using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class WhileStmt : AStmt
    {
        public readonly IExpr Guard;
        public readonly Block Block;

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Guard.FreeVars.Concat(Block.Select(s => s.UsedIdentifiers).Flatten());
            }
        }

        public WhileStmt(IExpr e, Block b, ISourceSpan locs) : base(locs)
        {
            this.Guard = e;
            this.Block = b;
        }
        
        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitWhileStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("while", Start);
            p.WritePunctuation("(");
            p.IncreaseIndent();
            Guard.PrettyPrint(p);
            p.WritePunctuation(")");
            p.DecreaseIndent();
            Block.PrettyPrint(p);
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<WhileStmt, S, R> VisitWhileStmt { get; }
    }
}
