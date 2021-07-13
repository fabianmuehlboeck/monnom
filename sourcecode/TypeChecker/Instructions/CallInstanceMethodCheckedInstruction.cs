using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    public class CallInstanceMethodCheckedInstruction : AValueInstruction
    {
        public IParameterizedSpecRef<IMethodSpec> Method { get; }
        public IRegister Receiver { get; }
        public IEnumerable<IRegister> Arguments { get; }
        public CallInstanceMethodCheckedInstruction(IParameterizedSpecRef<IMethodSpec> method, IRegister receiver, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            Method = method;
            Receiver = receiver;
            Arguments = arguments.ToList();
        }
        public IEnumerable<Language.IType> ActualParameters
        {
            get
            {
                return Method.GetOrderedArgumentList().Skip(Method.Element.OverallTypeParameterCount - Method.Element.TypeParameters.Count());
            }
        }
        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitCallInstanceMethodCheckedInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CallInstanceMethodCheckedInstruction, Arg, Ret> VisitCallInstanceMethodCheckedInstruction { get; }
    }
}
