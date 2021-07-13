using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class CallExpr: AExpr
    {
        public readonly IExpr Receiver;
        public readonly IEnumerable<IExpr> Args;

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Args.Select(a => a.FreeVars).Aggregate(Receiver.FreeVars, (agg, fv) => agg.Concat(fv));
            }
        }

        public CallExpr(IExpr receiver, IEnumerable<IExpr> args, ISourceSpan locs =null) : base(locs??new GenSourceSpan())
        {
            this.Receiver = receiver;
            this.Args = args;
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitCallExpr(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Receiver.PrettyPrint(p);
            p.WritePunctuation("(");
            p.PrintList(Args, ",");
            p.WritePunctuation(")");
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<CallExpr, S, R> VisitCallExpr { get; }
    }
}
