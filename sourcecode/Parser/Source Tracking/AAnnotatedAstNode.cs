using System;
using System.Collections.Generic;
using System.Text;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public abstract class AAnnotatedAstNode<T> : AAstNode
    {
        protected AAnnotatedAstNode(ISourceSpan locs = null) : base(locs)
        {
        }

        protected AAnnotatedAstNode(IToken start, IToken end) : base(start,end)
        {
        }

        protected AAnnotatedAstNode(IAstNode start, IToken end) : base(start, end)
        {
        }

        protected AAnnotatedAstNode(IToken start, IAstNode end): base(start, end)
        {
        }
        protected AAnnotatedAstNode(IAstNode start, IAstNode end): base(start, end)
        {
        }
        public T Annotation
        {
            get;
            set;
        }
    }
}
