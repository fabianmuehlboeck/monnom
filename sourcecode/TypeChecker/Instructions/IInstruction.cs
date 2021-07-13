using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public interface IInstruction
    {
        IEnumerable<IRegister> WriteRegisters { get; }
        Ret Visit<Arg, Ret>(IInstructionVisitor<Arg, Ret> visitor, Arg arg = default(Arg));
    }

    public partial interface IInstructionVisitor<in Arg, out Ret>
    {

    }
}
