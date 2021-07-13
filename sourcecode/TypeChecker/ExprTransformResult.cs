using System.Collections.Generic;
using System.Collections;

namespace Nom.TypeChecker
{
    internal class ExprTransformResult : IExprTransformResult
    {
        public ExprTransformResult(Language.IType type, IRegister register, IEnumerable<IInstruction> instructions, ISourceSpan locs =null)
        {
            Type = type;
            Register = register;
            Instructions = instructions;
            Locs = locs ?? new GenSourceSpan();
        }
        public Language.IType Type { get; }

        public IRegister Register { get; }

        public IEnumerable<IInstruction> Instructions { get; }

        public ISourceSpan Locs { get; }

        public IEnumerator<IInstruction> GetEnumerator()
        {
            return Instructions.GetEnumerator();
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new System.NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
