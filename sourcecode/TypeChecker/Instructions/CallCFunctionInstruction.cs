using Nom.Language;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.TypeChecker
{
    public class CallCFunctionInstruction : AValueInstruction
    {
        public ITDCFunction CFunction { get; }
        public IEnumerable<IType> TypeArguments { get; }
        public IEnumerable<IRegister> Arguments { get; }

        public CallCFunctionInstruction(String source, String name, IEnumerable<Language.ITypeParameterSpec> typeArgDecls, IEnumerable<Language.IType> typeArgs, IEnumerable<Language.IType> argTypes, Language.IType returnType, IEnumerable<IRegister> arguments, IRegister register) : base(register)
        {
            int argc = 0;
            CFunction = CFunctionRegistry.Instance.GetCFunction(source, name, new TypeParametersSpec(typeArgDecls), new ParametersSpec(argTypes.Select(at=>new StdLib.ParameterSpec("x"+(argc++).ToString(), at)).ToList()), returnType);
            TypeArguments = typeArgs;
            Arguments = arguments;
        }

        public override Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default)
        {
            return visitor.VisitCallCFunctionInstruction(this, arg);
        }
    }
    public partial interface IInstructionVisitor<in Arg, out Ret>
    {
        Func<CallCFunctionInstruction, Arg, Ret> VisitCallCFunctionInstruction { get; }
    }
}
