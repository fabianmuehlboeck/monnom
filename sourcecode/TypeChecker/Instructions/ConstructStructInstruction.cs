using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;

namespace Nom.TypeChecker
{
    public class ConstructStructInstruction : AValueInstruction
    {
        public ITDStruct Struct { get; }
        public IEnumerable<IType> TypeArguments { get; }
        public IEnumerable<IRegister> Arguments { get; }
        public ConstructStructInstruction(ITDStruct tdstruct, IEnumerable<IType> typeArguments, IEnumerable<IRegister> args, IRegister register) : base(register)
        {
            Struct = tdstruct;
            Arguments = args;
            TypeArguments = typeArguments;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitConstructStructInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<ConstructStructInstruction, Arg, Ret> VisitConstructStructInstruction { get; }
    }
}
