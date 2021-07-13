using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ITypeParametersSpec : ISpec, IEnumerable<ITypeParameterSpec>
    {
        IEnumerable<ITypeParameterSpec> Entries
        {
            get;
        }

        bool IsDisjoint(ITypeParametersSpec other);

        ITypeParametersSpec Merge(ITypeParametersSpec other);
    }
}
