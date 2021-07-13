using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ISignature
    {
        ITypeParametersSpec TypeParamters { get; }
        IParametersSpec Parameters { get; }
        IType ReturnType { get; }


    }
}
