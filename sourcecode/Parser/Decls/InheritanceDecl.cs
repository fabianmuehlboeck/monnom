using Nom.Language;
using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Parser
{
    public class InheritanceDecl : AAnnotatedAstNode<IParamRef<IInterfaceSpec, Language.IType>>
    {
        public readonly RefQName Parent;

        public InheritanceDecl(RefQName parent, ISourceSpan locs = null) : base(locs??parent.Locs)
        {
            this.Parent = parent;
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Parent.PrettyPrint(p);
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitInheritanceDecl(this, state);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<InheritanceDecl, S, R> VisitInheritanceDecl { get; }
    }
}
