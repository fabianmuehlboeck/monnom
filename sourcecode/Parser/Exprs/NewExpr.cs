using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class NewExpr : AExpr, IAnnotatedAstNode<Language.IParamRef<IClassSpec, Language.IType>>
    {
        public readonly NewCall NewCall;
        public readonly IEnumerable<IExpr> Args;

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Args.Select(a => a.FreeVars).Aggregate((IEnumerable<Identifier>)new List<Identifier>(), (agg, fv) => agg.Concat(fv));
            }
        }

        public Language.IParamRef<IClassSpec, Language.IType> Annotation { get; set; }

        public NewExpr(NewCall nc, IEnumerable<IExpr> args, ISourceSpan locs) : base(locs)
        {
            this.NewCall = nc;
            this.Args = args;
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitNewExpr(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            NewCall.PrettyPrint(p);
            p.WritePunctuation("(");
            p.PrintList(Args, ",");
            p.WritePunctuation(")");
            p.WriteWhitespace();
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<NewExpr, S, R> VisitNewExpr { get; }
    }
}
