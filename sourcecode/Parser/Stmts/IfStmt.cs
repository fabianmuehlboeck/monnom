using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class IfStmt : AStmt
    {
        private IExpr guard;
        private Block then;
        private IEnumerable<IExpr> eif_exprs;
        private IEnumerable<Block> eif_blocks;
        private IEnumerable<Block> else_blocks;

        private List<Tuple<IExpr, Block>> parts = new List<Tuple<IExpr, Block>>();

        public IExpr GuardExpression => guard;
        public Block ThenBlock => then;
        public IEnumerable<(IExpr, Block)> ElseIfs => eif_exprs.Zip(eif_blocks, (a, b) => (a, b));
        public IOptional<Block> ElseBlock => else_blocks.SingleOrDefault().InjectOptional();

        public IfStmt(IExpr e, Block then, IEnumerable<IExpr> eif_exprs, IEnumerable<Block> eif_blocks, IEnumerable<Block> else_blocks, ISourceSpan locs)
            : base(locs)
        {

            this.guard = e;
            this.then = then;
            this.eif_exprs = eif_exprs;
            this.eif_blocks = eif_blocks;
            this.else_blocks = else_blocks;


            parts.Add(new Tuple<IExpr, Block>(e, then));

            parts.AddRange(eif_exprs.Zip(eif_blocks, (ex, b) => new Tuple<IExpr, Block>(ex, b)));

            Block else_block = else_blocks.SingleOrDefault() ?? new Block(new List<IStmt>(), locs);
            parts.Add(new Tuple<IExpr, Block>(new BoolExpr("true", else_block.Locs), else_block));
        }
        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return parts.Select(p => p.Item1.FreeVars.Concat(p.Item2.UsedIdentifiers)).Flatten().Distinct();
            }
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitIfStmt(this, arg);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("if", Start);
            p.WritePunctuation("(");
            p.IncreaseIndent();
            GuardExpression.PrettyPrint(p);
            p.WritePunctuation(")");
            p.DecreaseIndent();
            ThenBlock.PrettyPrint(p);
            if(ElseBlock.HasElem)
            {
                p.WritePunctuation("else");
                ElseBlock.ActionBind(e => e.PrettyPrint(p));
            }
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<IfStmt, S, R> VisitIfStmt { get; }
    }
}
