using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.TypeChecker
{
    public interface IMethodDef : IMethodDecl
    {
        IEnumerable<IInstruction> Instructions { get; }
        int RegisterCount { get; }
    }
}
