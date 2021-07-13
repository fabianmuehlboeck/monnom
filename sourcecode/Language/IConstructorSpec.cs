using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IConstructorSpec : ICallableSpec, IMember
    {
        new IClassSpec Container { get; }
    }
}
