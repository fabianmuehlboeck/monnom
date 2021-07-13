using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface IConstructorDef : IConstructorSpec
    {
        IEnumerable<IInstruction> PreInstructions { get; }
        IEnumerable<IInstruction> PostInstructions { get; }
        IEnumerable<IRegister> SuperConstructorArgs { get; }
        int RegisterCount { get; }
    }
}
