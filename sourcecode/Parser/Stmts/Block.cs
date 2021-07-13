using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class Block : List<IStmt>, IAstNode, IStmt
    {
        
        public Block(IEnumerable<IStmt> statements, ISourceSpan locs) : base(statements)
        {
            this.Locs = locs;
        }
        public ISourcePos Start
        {
            get
            {
                return Locs.Start;
            }
        }

        public ISourcePos End
        {
            get
            {
                return Locs.End;
            }
        }

        public IEnumerable<Identifier> UsedIdentifiers
        {
            get
            {
                return (this).Select(s => s.UsedIdentifiers).Flatten();
            }
        }

        public ISourceSpan Locs
        {
            get;
            private set;
        }

        public string ReferencePosition => Locs.ReferencePosition;

        public Ret VisitAstNode<Arg,Ret>(IAstNodeVisitor<Arg,Ret> visitor, Arg arg = default(Arg))
        {
            return Visit(visitor, arg);
        }

        public Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBlock(this, arg);
        }

        public Block AsBlock()
        {
            return this;
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            p.WritePunctuation("{");
            p.IncreaseIndent();
            p.WriteLine();
            p.PrintList(this);
            p.DecreaseIndent();
            p.WriteLine();
            p.WritePunctuation("}");
            p.WriteLine();
        }
    }

    public partial interface IStmtVisitor<in S, out R>
    {
        Func<Block, S, R> VisitBlock { get; }
    }
}
