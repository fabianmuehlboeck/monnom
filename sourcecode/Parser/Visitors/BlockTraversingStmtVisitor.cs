using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class BlockTraversingStmtVisitor<Arg,Ret> : StmtVisitor<Arg,Ret>
    {
        public Func<IEnumerable<Ret>, Ret> Combinator
        {
            get;
            set;
        } = l => default(Ret);
        public override Func<Block, Arg, Ret> VisitBlock
        {
            get
            {
                return visitBlock;
            }

            set
            {
                throw new InvalidOperationException("Cannot set VisitBlock method of BlockTraversingStmtVisitor");
            }
        }

        private Ret visitBlock(Block b, Arg a)
        {
            return Combinator(b.Select(s => s.Visit(this, a)));
        }
    }
}
