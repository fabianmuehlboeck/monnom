using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IParameterizedSpec : ISpec
    {
        IOptional<IParameterizedSpec> ParameterizedParent { get; }
        ITypeParametersSpec TypeParameters { get; }
        int OverallTypeParameterCount { get; }

        ITypeParametersSpec AllTypeParameters { get; }

        IParameterizedSpecRef<IParameterizedSpec> GetAsRef();
    }
}
