using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface IStaticMethodDef : IStaticMethodSpec
    {
        IEnumerable<IInstruction> Instructions { get; }
        int RegisterCount { get; }
    }
}
