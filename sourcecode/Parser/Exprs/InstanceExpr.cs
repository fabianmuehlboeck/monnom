using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class InstanceExpr : AExpr
    {
        public readonly ClassType Class;
        public readonly Identifier InstanceName;
        public readonly IEnumerable<IExpr> Arguments;
        public InstanceExpr(ClassType cls, Identifier name, IEnumerable<IExpr> args, ISourceSpan locs =null) : base(locs)
        {
            Class = cls;
            InstanceName = name;
            Arguments = args.ToList();
        }

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                return Arguments.Select(a => a.FreeVars).Flatten();
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitInstanceExpr(this, state);
        }


    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<InstanceExpr, S, R> VisitInstanceExpr { get; }
    }

    public class DefaultInstanceExpr : InstanceExpr, IDefaultExpr
    {
        public readonly IEnumerable<IDefaultExpr> DefaultArguments;

        public DefaultInstanceExpr(ClassType cls, Identifier name, IEnumerable<IDefaultExpr> arguments, ISourceSpan locs =null):base(cls, name, arguments, locs)
        {
            DefaultArguments = arguments;
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultInstanceExpr(this, state);
        }
        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultInstanceExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultInstanceExpr, S, R> VisitDefaultInstanceExpr { get; }
    }
}
