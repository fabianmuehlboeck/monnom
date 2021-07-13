using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class StaticFieldDecl : AAnnotatedAstNode<IStaticFieldSpec>
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
        public IDefaultExpr InitExpr
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

        public StaticFieldDecl(Identifier ident, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = new DynamicType(ident.Locs);
            this.InitExpr = null;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }
        public StaticFieldDecl(Identifier ident, IType type, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = type;
            this.InitExpr = null;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }

        public StaticFieldDecl(Identifier ident, IDefaultExpr expr, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = new DynamicType(ident.Locs);
            this.InitExpr = expr;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }
        public StaticFieldDecl(Identifier ident, IType type, IDefaultExpr expr, VisibilityNode visibility = null, bool isReadOnly = false, ISourceSpan locs =null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Ident = ident;
            this.Type = type;
            this.InitExpr = expr;
            this.Visibility = visibility ?? VisibilityNode.Private;
            this.IsReadOnly = isReadOnly;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStaticFieldDecl(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Visibility.PrettyPrint(p);
            p.WriteKeyword("static");
            if (IsReadOnly)
            {
                p.WriteKeyword("readonly");
            }
            Type.PrettyPrint(p);
            Ident.PrettyPrint(p);
            if (InitExpr != null)
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
        Func<StaticFieldDecl, S, R> VisitStaticFieldDecl { get; }
    }
}

