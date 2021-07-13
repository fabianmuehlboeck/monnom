using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface ITypeConstant : IConstant
    {
        Language.IType Value { get; }
    }
}
