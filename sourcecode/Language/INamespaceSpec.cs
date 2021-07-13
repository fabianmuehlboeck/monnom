using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface INamespaceSpec : IParameterizedSpec
    {
        IOptional<INamespaceSpec> ParentNamespace
        {
            get;
        }
        String Name
        {
            get;
        }

        IEnumerable<INamespaceSpec> Namespaces
        {
            get;
        }

        IEnumerable<IInterfaceSpec> Interfaces
        {
            get;
        }

        IEnumerable<IClassSpec> Classes
        {
            get;
        }

        IEnumerable<INamespaceSpec> Children
        {
            get;
        }
        IEnumerable<INamespaceSpec> PublicChildren
        {
            get;
        }
        IEnumerable<INamespaceSpec> ProtectedChildren
        {
            get;
        }

        string FullQualifiedName { get; }

        /*bool HasChild<T>(IQName<IArgIdentifier<String,T>> name);
        bool HasChild<T>(IArgIdentifier<String,T> identifier);
        bool HasChild(String name, int argcount);
        INamespaceSpec GetChild<T>(IQName<IArgIdentifier<String,T>> name);
        INamespaceSpec GetChild<T>(IArgIdentifier<String,T> identifier);
        INamespaceSpec GetChild(String name, int argcount);*/
        
        Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg));
    }
}
