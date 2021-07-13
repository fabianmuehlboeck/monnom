using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class FunDef:AAstNode
    {
        public FunDef(Identifier name, IEnumerable<VarDecl> args, IEnumerable<IType> returns, IEnumerable<IStmt> code, ISourceSpan locs):base(locs)
        {

        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitFunDef(this, state);
        }

    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<FunDef, S, R> VisitFunDef { get; }
    }
}

