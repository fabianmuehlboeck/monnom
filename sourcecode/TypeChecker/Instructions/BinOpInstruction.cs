using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class BinOpInstruction : AValueInstruction
    {
        public IRegister Left { get; }
        public IRegister Right { get; }
        public Parser.BinaryOperator Operator { get; }
        public BinOpInstruction(IRegister left, IRegister right, Parser.BinaryOperator op, IRegister register) : base(register)
        {
            Left = left;
            Right = right;
            Operator = op;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitBinOpInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<BinOpInstruction, Arg, Ret> VisitBinOpInstruction { get; }
    }
}
