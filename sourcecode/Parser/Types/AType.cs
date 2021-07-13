using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public abstract class AType : AAnnotatedAstNode<Language.IType>, IType
    {

        public string ReferencePosName => ReferenceName + " (" + ReferencePosition + ")";

        public abstract string ReferenceName { get; }

        protected AType(ISourceSpan locs):base(locs)
        {
        }

   
        public virtual T Visit<T>(TypeVisitor<T> visitor)
        {
            return Visit(visitor, null);
        }


        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return Visit(visitor, state);
        }

        public abstract R Visit<S, R>(ITypeVisitor<S, R> visitor, S state);
    }
}
