using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public interface IIdentExprAnnotation
    {

        //TODO: remove hack by tracing origin of variable binding
        IOptional<string> VariableName { get; }
    }

    public class IdentExprAnnotation : IIdentExprAnnotation
    {
        public IdentExprAnnotation(string varName)
        {
            VariableName = varName.InjectOptional();
        }
        public IOptional<string> VariableName { get; } = Optional<string>.Empty;
    }

    public class IdentExpr : AAnnotatedExpr<IIdentExprAnnotation>, IAccessorExpr
    {
        public readonly Identifier Name;
        public readonly IExpr Accessor;
        public IdentExpr(Identifier i, IExpr acc = null) : base(i.Locs)
        {
            this.Name = i;
            this.Accessor = acc;
        }

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                yield return Name;
                yield break;
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            if(Accessor!=null)
            {
                Accessor.PrettyPrint(p);
                p.WritePunctuation(".");
            }
            Name.PrettyPrint(p);
            p.WriteWhitespace();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitIdentExpr(this, state);
        }

    }

    public class DefaultIdentExpr : IdentExpr, IDefaultExpr
    {
        public DefaultIdentExpr(Identifier i, IExpr acc = null) : base(i, acc)
        {
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultIdentExpr(this, state);
        }

        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state = default)
        {
            return visitor.VisitDefaultIdentExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultIdentExpr, S, R> VisitDefaultIdentExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<IdentExpr, S, R> VisitIdentExpr { get; }
    }
}