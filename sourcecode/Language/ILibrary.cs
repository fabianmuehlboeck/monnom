using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface ILibrary
    {
        string Name
        {
            get;
        }

        Version Version
        {
            get;
        }

        INamespaceSpec GlobalNamespace
        {
            get;
        }

        IEnumerable<IParamRef<INamespaceSpec, P>> FindVarargsChildren<P>(IArgIdentifier<string, P> name) where P : ITypeArgument, ISubstitutable<P>;
    }
}
