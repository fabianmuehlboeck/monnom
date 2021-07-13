using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public abstract class AAstNode : IAstNode, IPositionalReference
    {
        public ISourceSpan Locs {get;private set;}

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

        public string ReferencePosition => Locs.ReferencePosition;

        protected AAstNode(ISourceSpan locs=null)
        {
            Locs = locs??new LibSourcePos().AsSourceSpan();
        }

        protected AAstNode(IToken start, IToken end)
        {
            Locs = new FileSourceSpan(start.ToSourcePos(), end.ToSourcePos());
        }

        protected AAstNode(IAstNode start, IToken end)
        {
            Locs = start.Start.SpanTo(end.ToSourcePos());
        }

        protected AAstNode(IToken start, IAstNode end)
        {
            Locs = start.ToSourcePos().SpanTo(end.End);
        }
        protected AAstNode(IAstNode start, IAstNode end)
        {
            Locs = start.Start.SpanTo(end.End);
        }

        public abstract R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state);
        public abstract void PrettyPrint(PrettyPrinter p);
    }
}
