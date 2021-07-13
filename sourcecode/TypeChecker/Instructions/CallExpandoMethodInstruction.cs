using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.TypeChecker
{
    public class CallExpandoMethodInstruction : AValueInstruction
    {
        public IRegister Receiver { get; }
        public string MethodName { get; }
        public IEnumerable<Language.IType> TypeArguments { get;}

        public IEnumerable<IRegister> Arguments { get; }
 
        public CallExpandoMethodInstruction(IRegister receiver, string methodName, IEnumerable<Language.IType> typeArguments, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            Receiver=receiver;
            MethodName=methodName;
            TypeArguments = typeArguments.ToList();
            Arguments = arguments.ToList();
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitCallExpandoMethodInstruction(this, arg);
        }
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CallExpandoMethodInstruction, Arg, Ret> VisitCallExpandoMethodInstruction { get; }
    }
}
