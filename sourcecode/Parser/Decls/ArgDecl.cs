using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class ArgDecl:AAnnotatedAstNode<Language.IParameterSpec>
    {
        public Identifier Name
        {
            get;
            private set;
        }
        public IEnumerable<IType> Types
        {
            get
            {
                yield return Type;
            }
        }

        public IType Type
        {
            get;
            private set;
        }
        public ArgDecl(Identifier ident, IType type, ISourceSpan locs)
            : base(locs)
        {
            Name = ident;
            Type = type;
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitArgDecl(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            if(Type!=null)
            {
                Type.PrettyPrint(p);
                p.WriteWhitespace();
            }
            Name.PrettyPrint(p);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<ArgDecl, S, R> VisitArgDecl { get; }
    }
}
