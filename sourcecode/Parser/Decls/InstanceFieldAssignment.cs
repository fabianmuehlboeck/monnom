using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class InstanceFieldAssignment : AAstNode
    {
        public readonly Identifier FieldName;
        public readonly IDefaultExpr Expr;
        public InstanceFieldAssignment(Identifier fieldName, IDefaultExpr expr, ISourceSpan locs = null) : base(locs)
        {
            FieldName = fieldName;
            Expr = expr;
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitInstanceFieldAssignment(this, state);
        }

    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<InstanceFieldAssignment, S, R> VisitInstanceFieldAssignment { get; }
    }
}

