using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface INamespace : INamespaceSpec
    {
        new IEnumerable<INamespace> Namespaces { get; }
        new IEnumerable<IInterface> Interfaces { get; }
        new IEnumerable<IClass> Classes { get; }
    }
}
