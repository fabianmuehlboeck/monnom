using Nom.Language;
using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class TypeArgDecl : AAnnotatedAstNode<ITypeParameterSpec>, INamedReference
    {
        public TypeArgDecl(Identifier name, IType upperBound = null, IType lowerBound = null, ISourceSpan locs=null) : base(locs)
        {
            this.Name = name;
            //this.Variance = var;
            this.UpperBound = upperBound;
            this.LowerBound = lowerBound;
        }

        public Identifier Name
        {
            get;
            private set;
        }


        public IType UpperBound
        {
            get;
            private set;
        }

        public IType LowerBound
        {
            get;
            private set;
        }

        public string ReferenceName => Name.Name;

        public override void PrettyPrint(PrettyPrinter p)
        {
            Name.PrettyPrint(p);
            if(UpperBound!=null)
            {
                p.WriteKeyword("extends");
                UpperBound.PrettyPrint(p);
            }
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitTypeArgDecl(this, state);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<TypeArgDecl, S, R> VisitTypeArgDecl { get; }
    }
}
