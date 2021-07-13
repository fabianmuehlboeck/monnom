using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public abstract class AOpExpr : AAstNode, IExpr
    {
        public readonly IExpr Left;
        public readonly IExpr Right;
        protected AOpExpr(IExpr left,IExpr right, ISourceSpan locs) : base(locs??left.Start.SpanTo(right.End))
        {
            this.Left = left;
            this.Right = right;
        }

        public IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Left.FreeVars.Concat(Right.FreeVars);
            }
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }
        public abstract R Visit<S, R>(IExprVisitor<S, R> visitor, S state);


        public virtual bool IsAtomic
        {
            get
            {
                return true;
            }
        }

        public IType TypeAnnotation { get; set; }
    }
}
