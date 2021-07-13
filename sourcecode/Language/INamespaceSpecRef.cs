using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface INamespaceSpecRef : IParameterizedSpecRef<INamespaceSpec>
    {
        IOptional<INamespaceSpecRef> ParentSpecRef
        {
            get;
        }

        IOptional<INamespaceSpecRef> FindPublicChild<T>(IArgIdentifier<String, IQName<T>> name, INamespaceLookupContext context) where T : INamedReference;
        IOptional<INamespaceSpecRef> FindPublicChild<T>(IQName<IArgIdentifier<String, IQName<T>>> name, INamespaceLookupContext context) where T : INamedReference;
        IOptional<INamespaceSpecRef> FindPrivateChild<T>(IArgIdentifier<String, IQName<T>> name, INamespaceLookupContext context) where T : INamedReference;
        IOptional<INamespaceSpecRef> FindPrivateChild<T>(IQName<IArgIdentifier<String, IQName<T>>> name, INamespaceLookupContext context) where T : INamedReference;
        IOptional<INamespaceSpecRef> FindProtectedChild<T>(IArgIdentifier<String, IQName<T>> name, INamespaceLookupContext context) where T : INamedReference;
        IOptional<INamespaceSpecRef> FindProtectedChild<T>(IQName<IArgIdentifier<String, IQName<T>>> name, INamespaceLookupContext context) where T : INamedReference;

    }
}
