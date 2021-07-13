using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IInterfaceSpec : INamespaceSpec
    {
        bool IsShape
        {
            get;
        }
        bool IsExpando
        {
            get;
        }

        Visibility Visibility
        {
            get;
        }

        ILibrary Library
        {
            get;
        }

        /// <summary>
        /// Declared immediate interface supertypes
        /// </summary>
        IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements
        {
            get;
        }


        IEnumerable<IMethodSpec> Methods
        {
            get;
        }
        new IEnumerable<INamespaceSpec> Children
        {
            get;
        }

        INamedType Instantiate(ITypeEnvironment<ITypeArgument> args);
    }
}
