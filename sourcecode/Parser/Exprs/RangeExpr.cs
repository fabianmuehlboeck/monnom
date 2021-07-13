using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class RangeExpr : AExpr
    {
        public readonly IExpr RangeStart, RangeEnd, RangeStep;
        public RangeExpr(IExpr end, IExpr start=null, IExpr step=null, ISourceSpan locs =null) : base(locs??new GenSourceSpan())
        {
            this.RangeEnd = end;
            this.RangeStart = start ?? new IntExpr(0);
            this.RangeStep = step ?? new IntExpr(1);
        }

        public override IEnumerable<Identifier> FreeVars
        {
            get
            {
                IEnumerable<Identifier> ret = RangeEnd.FreeVars;
                ret = ret.Concat(RangeStart.FreeVars.Except(ret));
                ret = ret.Concat(RangeStep.FreeVars.Except(ret));
                return ret;
            }
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.Write("[", Start);
            RangeStart.PrettyPrint(p);
            p.WritePunctuation("..");
            RangeEnd.PrettyPrint(p);
            p.WritePunctuation(":");
            RangeStep.PrettyPrint(p);
            p.WritePunctuation("]");
            p.WriteWhitespace();
        }

        //public override TDTransformResult TransformTD(TDLookup available)
        //{
        //    TDTransformResult startTR = RangeStart.TransformTD(available);
        //    TDTransformResult endTR = RangeEnd.TransformTD(available);
        //    TDTransformResult stepTR = RangeStep.TransformTD(available);

        //    if (!startTR.Type.IsOptimisticSubtype(TDClassType.IntType.ToStaticTR()))
        //    {
        //        CompilerOutput.RegisterException(new TypecheckException(RangeStart.Locs, "Int expected, found " + startTR.Type.ToString()));
        //    }
        //    if (!endTR.Type.IsOptimisticSubtype(TDClassType.IntType.ToStaticTR()))
        //    {
        //        CompilerOutput.RegisterException(new TypecheckException(RangeEnd.Locs, "Int expected, found " + endTR.Type.ToString()));
        //    }
        //    if (!stepTR.Type.IsOptimisticSubtype(TDClassType.IntType.ToStaticTR()))
        //    {
        //        CompilerOutput.RegisterException(new TypecheckException(RangeStep.Locs, "Int expected, found " + stepTR.Type.ToString()));
        //    }

        //    NewInstruction instruction = new NewInstruction(RangeClassDef.Instance.TDClassDef, new List<Register> { startTR.Register, endTR.Register, stepTR.Register }, RangeClassDef.Instance.TDClassDef.GetConstructor(available.Context, new List<TDTypeRange> { startTR.Type, endTR.Type, stepTR.Type }), this);
        //    return new NominalGradual.TDTransformResult(startTR.Concat(endTR).Concat(stepTR).Snoc(instruction), instruction.Register, available.GetClassType(new NominalGradual.QName("Range", this.Locs)).ToStaticTR());
        //}
        public override R Visit<S, R>(IExprVisitor<S, R> visitor, S state)
        {
            return visitor.VisitRangeExpr(this, state);
        }
    }

    public partial interface IExprVisitor<in S, out R>
    {
        Func<RangeExpr, S, R> VisitRangeExpr { get; }
    }
}
