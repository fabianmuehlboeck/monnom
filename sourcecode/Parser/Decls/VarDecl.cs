using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class VarDecl : AAstNode
    {
        public Identifier Name
        {
            get;
            private set;
        }
        public IType Type
        {
            get;
            private set;
        }
        public VarDecl(Identifier ident, IType type=null, ISourceSpan locs=null) : base(locs??ident.Locs)
        {
            Name = ident;
            Type = type ?? new DynamicType(locs);
        }


        public ArgDecl ToArgDecl()
        {
            return new ArgDecl(Name, Type, Locs);
        }
        

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitVarDecl(this, state);
        }
        public override void PrettyPrint(PrettyPrinter p)
        {
            if (Type != null)
            {
                Type.PrettyPrint(p);
                p.WriteWhitespace();
            }
            Name.PrettyPrint(p);
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<VarDecl, S, R> VisitVarDecl { get; }
    }
}

