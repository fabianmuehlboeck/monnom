using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public class CastInstruction : AValueInstruction
    {
        public IRegister Argument { get; }
        public Language.IType Type { get; }
        public CastInstruction(Language.IType type, IRegister argument, IRegister register) : base(register)
        {
            Type = type;
            Argument = argument;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitCastInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CastInstruction, Arg, Ret> VisitCastInstruction { get; }
    }
}
