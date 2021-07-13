using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IParameterSpec : ISpec
    {
        String Name
        {
            get;
        }

        IType Type
        {
            get;
        }
    }
}
