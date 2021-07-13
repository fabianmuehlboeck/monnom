using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class FieldDecl : AAstNode
    {
        public Identifier Ident
        {
            get;
            private set;
        }
        public IType Type
        {
            get;
            private set;
        }
        public IExpr InitExpr
        {
            get;
            private set;
        }
        public ClassDef Class
        {
            get;
            set;
        }

        public VisibilityNode Visibility
        {
            get;
            private set;
        }

        public bool IsReadOnly
        {
            get;
            private set;
        }

        public FieldDecl(Identifier ident, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs=null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = new DynamicType(ident.Locs);
            this.InitExpr = null;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }
        public FieldDecl(Identifier ident, IType type, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs =null)
            : base(locs?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = type;
            this.InitExpr = null;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }

        public FieldDecl(Identifier ident, IExpr expr, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs =null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = new DynamicType(ident.Locs);
            this.InitExpr = expr;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }
        public FieldDecl(Identifier ident, IType type, IExpr expr, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs =null) 
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = type;
            this.InitExpr = expr;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }
        
        public ArgDecl ToArgDecl()
        {
            return new ArgDecl(Ident, Type, Locs);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitFieldDecl(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.WriteWhitespace();
            if(IsReadOnly)
            {
                p.WriteKeyword("readonly");
            }
            p.WriteWhitespace();
            Type.PrettyPrint(p);
            p.WriteWhitespace();
            Ident.PrettyPrint(p);
            if(InitExpr!=null)
            {
                p.WriteKeyword("=");
                InitExpr.PrettyPrint(p);
            }
            p.WritePunctuation(";");
            p.WriteLine();
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<FieldDecl, S, R> VisitFieldDecl { get; }
    }
}

