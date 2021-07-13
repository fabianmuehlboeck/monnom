using System;
using System.Collections.Generic;
using System.Text;

using Nom.Language;

namespace Nom.TypeChecker
{
    public class ReadLambdaFieldInstruction : AValueInstruction
    {
        public TDLambdaField Field { get; }
        public ReadLambdaFieldInstruction(TDLambdaField field, IRegister register) : base(register)
        {
            Field = field;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitReadLambdaFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReadLambdaFieldInstruction, Arg, Ret> VisitReadLambdaFieldInstruction { get; }
    }
}
