using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class UnaryOpInstruction : AValueInstruction
    {
        public IRegister Arg { get; }
        public Parser.UnaryOperator Operator { get; }
        public UnaryOpInstruction(IRegister arg, Parser.UnaryOperator op, IRegister register) : base(register)
        {
            Arg = arg;
            Operator = op;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitUnaryOpInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<UnaryOpInstruction, Arg, Ret> VisitUnaryOpInstruction { get; }
    }
}
