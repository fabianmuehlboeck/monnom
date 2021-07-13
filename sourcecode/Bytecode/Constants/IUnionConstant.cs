using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface IUnionConstant : IConstant, ITypeConstant
    {
        TypeListConstant ComponentsConstant
        {
            get;
            set;
        }
    }
}
