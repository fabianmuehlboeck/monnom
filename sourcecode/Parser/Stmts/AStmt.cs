using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public abstract class AStmt : AAstNode, IStmt
    {
        public AStmt(ISourceSpan locs)
            : base(locs)
        {

        }

        public abstract IEnumerable<Identifier> UsedIdentifiers
        {
            get;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }

        public abstract Ret Visit<Arg, Ret>(IStmtVisitor<Arg, Ret> visitor, Arg arg = default(Arg));

        public virtual Block AsBlock()
        {
            return new Block(this.Singleton(), Locs);
        }
    }
}
