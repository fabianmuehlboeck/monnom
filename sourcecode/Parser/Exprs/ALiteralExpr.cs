using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public abstract class ALiteralExpr : AAstNode, IExpr
    {
        public string Value { get; protected set; }
        
        public IEnumerable<Identifier> FreeVars
        {
            get
            {
                return new List<Identifier>();
            }
        }
        public virtual bool IsAtomic
        {
            get
            {
                return true;
            }
        }

        public IType TypeAnnotation { get; set; }

        protected ALiteralExpr(IToken tok, String moreText="")
            : base(tok, tok)
        {
            Value = moreText+tok.Text;
        }

        protected ALiteralExpr(String value, ISourceSpan locs):base(locs)
        {
            Value = value;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }
        public abstract R Visit<S, R>(IExprVisitor<S, R> visitor, S state);

    }
}
