using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class IfObjStmt : AStmt
    {
        public readonly Identifier Ident;
        public readonly Block ThenBlock;
        public readonly Optional<Block> ElseBlock;
        public IfObjStmt(Identifier i, Block then, Block elseBlock, ISourceSpan locs) : base(locs)
        {
            Ident = i;
            ThenBlock = then;
            ElseBlock = elseBlock.InjectOptional();
        }

        public override IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return Ident.Singleton().Concat(ThenBlock.UsedIdentifiers).Concat(ElseBlock.Extract(b => b.UsedIdentifiers, new List<Identifier>()));
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.WriteKeyword("ifobj");
            p.WritePunctuation("(");
            p.IncreaseIndent();
            Ident.PrettyPrint(p);
            p.WritePunctuation(")");
            p.DecreaseIndent();
            ThenBlock.PrettyPrint(p);
            if (ElseBlock.HasElem)
            {
                p.WritePunctuation("else");
                ElseBlock.ActionBind(e => e.PrettyPrint(p));
            }
        }

        public override Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitIfObjStmt(this, arg);
        }
    }
    public partial interface IStmtVisitor<in S, out R>
    {
        Func<IfObjStmt, S, R> VisitIfObjStmt { get; }
    }
}
