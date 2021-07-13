using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IParametersSpec : ISpec
    {
        IEnumerable<IParameterSpec> Entries
        {
            get;
        }

        bool IsDisjoint(IParametersSpec other);

        IParametersSpec Merge(IParametersSpec other);

        bool WouldAccept(ITypeEnvironment<IType> env, IEnumerable<IType> args, bool optimistic = true);
    }
}
