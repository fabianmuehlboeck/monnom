using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface IIntersectionConstant : IConstant, ITypeConstant
    {
        IConstantRef<TypeListConstant> ComponentsConstant
        {
            get;
        }
    }
}
