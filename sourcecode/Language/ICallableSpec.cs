using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ICallableSpec : IParameterizedSpec
    {
        IParametersSpec Parameters
        {
            get;
        }
        IType ReturnType
        {
            get;
        }
    }
}
