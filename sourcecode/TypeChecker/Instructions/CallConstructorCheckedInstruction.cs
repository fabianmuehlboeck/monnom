using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Nom.TypeChecker
{
    public class CallConstructorCheckedInstruction : AValueInstruction
    {
        public Language.IParameterizedSpecRef<Language.IConstructorSpec> Constructor { get; }
        public IEnumerable<IRegister> Arguments { get; }
        public CallConstructorCheckedInstruction(Language.IParameterizedSpecRef<Language.IConstructorSpec> constructor, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            Constructor = constructor;
            Arguments = arguments;
        }

        public IEnumerable<Language.IType> ActualParameters
        {
            get
            {
                return Constructor.GetOrderedArgumentList();
            }
        }
        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitCallConstructorCheckedInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CallConstructorCheckedInstruction, Arg, Ret> VisitCallConstructorCheckedInstruction { get; }
    }
}
