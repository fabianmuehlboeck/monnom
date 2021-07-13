using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class ReadInstanceFieldInstruction : AValueInstruction
    {
        public IFieldSpec Field { get; }
        public IRegister Receiver { get; }
        public ReadInstanceFieldInstruction(IFieldSpec field, IRegister receiver, IRegister register) : base(register)
        {
            Field = field;
            Receiver = receiver;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReadInstanceFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReadInstanceFieldInstruction, Arg, Ret> VisitReadInstanceFieldInstruction { get; }
    }
}
