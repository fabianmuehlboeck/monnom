using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface INamespaceLookupContext
    {
        IOptional<INamespaceLookupContext> ParentContext
        {
            get;
        }
        INamespaceSpecRef LookupRoot
        {
            get;
        }
        IEnumerable<INamespaceSpecRef> ProtectedContexts
        {
            get;
        }

        IEnumerable<INamespaceSpecRef> UsingContexts
        {
            get;
        }

        IEnumerable<ITypeParameterSpec> TypeParameters
        {
            get;
        }

        IOptional<INamespaceSpecRef> FindChild(IArgIdentifier name);
        IOptional<INamespaceSpecRef> FindChild(IQName name);

        INamespaceLookupContext PushParams(IEnumerable<ITypeParameterSpec> typeParams);
        INamespaceLookupContext Push(INamespaceSpecRef newRoot, IEnumerable<INamespaceSpecRef> protecteds);
    }
}
