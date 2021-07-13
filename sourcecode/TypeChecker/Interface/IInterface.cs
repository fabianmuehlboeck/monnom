using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.TypeChecker
{
    public interface IInterface : IInterfaceSpec
    {
        new IEnumerable<IInterface> Interfaces { get; }
    }
}
