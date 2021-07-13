using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IMember
    {
        INamespaceSpec Container { get; }
        Visibility Visibility { get; }
    }
}
