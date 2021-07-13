using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class CallStaticMethodCheckedInstruction : AValueInstruction
    {
        public IParameterizedSpecRef<IStaticMethodSpec> Method { get; }
        public IEnumerable<IRegister> Arguments { get; }
        public CallStaticMethodCheckedInstruction(IParameterizedSpecRef<IStaticMethodSpec> method, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            Method = method;
            Arguments = arguments;
        }

        public IEnumerable<Language.IType> ActualParameters
        {
            get
            {
                return Method.GetOrderedArgumentList();
            }
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitCallStaticMethodCheckedInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CallStaticMethodCheckedInstruction, Arg, Ret> VisitCallStaticMethodCheckedInstruction { get; }
    }
}
