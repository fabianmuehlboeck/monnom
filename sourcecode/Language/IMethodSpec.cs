using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IMethodSpec : ICallableSpec, IMember
    {
        String Name
        {
            get;
        }
        bool IsFinal
        {
            get;
        }
        bool IsVirtual
        {
            get;
        }
        bool Overrides(IMethodSpec other, ITypeEnvironment<IType> substitutions);
    }
}
