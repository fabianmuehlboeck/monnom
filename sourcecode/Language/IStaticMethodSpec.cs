using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IStaticMethodSpec : ICallableSpec, IMember
    {
        String Name
        {
            get;
        }

    }
}
