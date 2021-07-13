using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface IStringConstant : IConstant
    {
        String Value
        {
            get;
        }
    }
}
