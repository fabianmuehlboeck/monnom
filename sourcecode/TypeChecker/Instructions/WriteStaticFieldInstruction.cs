using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class WriteStaticFieldInstruction : AInstruction
    {
        public IRegister Register { get; }
        public IStaticFieldSpec Field { get; }
        public ITypeEnvironment<IType> TypeArguments { get; }
        public WriteStaticFieldInstruction(IStaticFieldSpec field, ITypeEnvironment<IType> typeArgs, IRegister register)
        {
            Field = field;
            TypeArguments = typeArgs;
            Register = register;
        }
        public override IEnumerable<IRegister> WriteRegisters { get { yield break; } }
        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitWriteStaticFieldInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<WriteStaticFieldInstruction, Arg, Ret> VisitWriteStaticFieldInstruction { get; }
    }
}
