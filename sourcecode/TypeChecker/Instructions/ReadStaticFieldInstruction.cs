using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class ReadStaticFieldInstruction : AValueInstruction
    {
        public IStaticFieldSpec Field { get; }
        public ITypeEnvironment<IType> TypeArguments { get; }
        public ReadStaticFieldInstruction(IStaticFieldSpec field, ITypeEnvironment<IType> typeArgs, IRegister register) : base(register)
        {
            Field = field;
            TypeArguments = typeArgs;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitReadStaticFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ReadStaticFieldInstruction, Arg, Ret> VisitReadStaticFieldInstruction { get; }
    }
}
