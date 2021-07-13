using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Nom.Language;

namespace Nom.TypeChecker
{
    public class CreateClosureInstruction : AValueInstruction
    {
        public TDLambda Lambda { get; }
        public IEnumerable<IType> TypeArgs { get; }
        public IEnumerable<IRegister> Arguments { get; }
        public CreateClosureInstruction(TDLambda lambda, IEnumerable<IType> typeArgs, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            Lambda=lambda;
            TypeArgs=typeArgs;
            Arguments = arguments;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitCreateClosureInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CreateClosureInstruction, Arg, Ret> VisitCreateClosureInstruction { get; }
    }
}
