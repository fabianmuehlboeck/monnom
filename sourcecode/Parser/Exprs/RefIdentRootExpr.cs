using System;
using System.Collections.Generic;
using System.Text;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class RefIdentRootExpr : AQName<IArgIdentifier<Identifier, IType>>, IExpr
    {
        
        public RefIdentRootExpr(Identifier assembly, Identifier name, IEnumerable<IType> args, IToken cc, IToken rangle) : base(cc!=null,(assembly?.Start??(cc==null?name.Start:cc.ToSourcePos())).SpanTo(rangle), new RefIdentifier(name, args, name.Start.SpanTo(rangle)))
        {

        }

        public IEnumerable<Identifier> FreeVars
        {
            get
            {
                yield break;
            }
        }

        public bool IsAtomic => throw new NotImplementedException();

        public IType TypeAnnotation { get; set; }

        public R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitRefIdentRootExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<RefIdentRootExpr, S, R> VisitRefIdentRootExpr { get; }
    }
}
