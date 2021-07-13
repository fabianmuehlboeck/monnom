using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.Parser
{
    public class StructFieldDecl : AAnnotatedAstNode<IFieldSpec>
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
        public StructFieldDecl(IType type, Identifier name, IExpr initExpr) : base((type?.Start ?? name.Start).SpanTo(initExpr.End))
        {
            Ident = name;
            Type = type ?? new DynamicType(name.Locs);
            InitExpr = initExpr;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitStructFieldDecl(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Type.PrettyPrint(p);
            p.WriteWhitespace();
            p.IncreaseIndent();
            Ident.PrettyPrint(p);
            if(InitExpr!=null)
            {
                p.WriteKeyword("=");
                InitExpr.PrettyPrint(p);
            }
            p.DecreaseIndent();
        }
    }
    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<StructFieldDecl, S, R> VisitStructFieldDecl { get; }
    }
}
