using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class WriteInstanceFieldInstruction : AInstruction
    {
        public IFieldSpec Field { get; }
        public IRegister Receiver { get; }
        public IRegister Value { get; }

        public WriteInstanceFieldInstruction(IFieldSpec field, IRegister receiver, IRegister value)
        {
            Field = field;
            Receiver = receiver;
            Value = value;
        }

        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }
        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitWriteInstanceFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<WriteInstanceFieldInstruction, Arg, Ret> VisitWriteInstanceFieldInstruction { get; }
    }
}
