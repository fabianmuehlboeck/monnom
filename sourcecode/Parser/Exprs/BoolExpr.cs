using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class BoolExpr:ALiteralExpr, IExpr
    {
        public BoolExpr(IToken tok):base(tok)
        {

        }

        public BoolExpr(String value, ISourceSpan locs):base(value, locs)
        {

        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write(Value, Start);
            p.WriteWhitespace();
        }

        //public override TDTransformResult TransformTD(TDLookup available)
        //{
        //    ConstantInstruction instruction = ConstantInstruction.GetBoolConstantInstruction(Value, this);
        //    return new TDTransformResult(instruction.Singleton(), instruction.Register, instruction.Register.Type);
        //}

        //public ITDDefaultValue TransformTDDefault(TDLookup lookup)
        //{
        //    return new TDDefaultValueBool(Value, Locs);
        //}

        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitBoolExpr(this, state);
        }
    }

    public class DefaultBoolExpr : BoolExpr, IDefaultExpr
    {
        public DefaultBoolExpr(IToken token)
            : base(token)
        {

        }

        public DefaultBoolExpr(string value, ISourceSpan locs = null)
            : base(value, locs)
        {
        }
        public R Visit<S, R>(IDefaultExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultBoolExpr(this, state);
        }
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitDefaultBoolExpr(this, state);
        }
    }

    public partial interface IDefaultExprVisitor<in S, out R>
    {
        Func<DefaultBoolExpr, S, R> VisitDefaultBoolExpr { get; }
    }
    public partial interface IExprVisitor<in S, out R>
    {
        Func<BoolExpr, S, R> VisitBoolExpr { get; }
    }
}
