using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public abstract class AInstruction : IInstruction
    {
        public AInstruction()
        {

        }

        public abstract IEnumerable<IRegister> WriteRegisters { get; }

        public abstract Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg));
    }
}
