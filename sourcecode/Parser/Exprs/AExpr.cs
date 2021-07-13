using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public abstract class AExpr : AAstNode,IExpr
    {
        public AExpr(ISourceSpan locs)
            : base(locs)
        {

        }

        public abstract IEnumerable<Identifier> FreeVars
        {
            get;
        }

        public virtual bool IsAtomic
        {
            get
            {
                return false;
            }
        }

        public virtual IType TypeAnnotation { get; set; }


        R IAstNode.VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }
        public abstract R Visit<S, R>(IExprVisitor<S, R> visitor, S state);
    }

    public abstract class AAnnotatedExpr<T> : AAnnotatedAstNode<T>, IExpr
    {
        public AAnnotatedExpr(ISourceSpan locs)
            : base(locs)
        {

        }

        public abstract IEnumerable<Identifier> FreeVars
        {
            get;
        }

        public virtual bool IsAtomic
        {
            get
            {
                return false;
            }
        }

        public IType TypeAnnotation { get; set; }


        R IAstNode.VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }
        public abstract R Visit<S, R>(IExprVisitor<S, R> visitor, S state);
    }
}
