using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ISubstitutable<out T>
    {
        T Substitute(ITypeParameterSpec param, IType type);
        T Substitute<E>(ITypeEnvironment<E> env) where E : ITypeArgument, ISubstitutable<E>;
    }
}
