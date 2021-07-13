using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class ListExpr : AExpr
    {
        public readonly IEnumerable<IExpr> Elements;
        public ListExpr(IEnumerable<IExpr> elems, ISourceSpan locs) : base(locs)
        {
            Elements = elems.ToList();
        }

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Elements.Select(e => e.FreeVars).Flatten();
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitListExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<ListExpr, S, R> VisitListExpr { get; }
    }

    public class DefaultListExpr : ListExpr, IDefaultExpr
    {
        public readonly IEnumerable<IDefaultExpr> DefaultElements;
        public DefaultListExpr(IEnumerable<IDefaultExpr> elems, ISourceSpan locs):base(elems,locs)
        {
            DefaultElements = elems.ToList();
        }

        public new IEnumerable<IDefaultExpr> Elements => DefaultElements;

        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultListExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultListExpr, S, R> VisitDefaultListExpr { get; }
    }
}
