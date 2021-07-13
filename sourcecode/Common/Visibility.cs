using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    /// <summary>
    /// TODO: Really, accesses to internal things need to be like accesses to private things that happen to be visible
    /// ProtectedInternal can go either way if both protected and internal access are given
    /// </summary>
    public enum Visibility : byte
    {
        Private = 1,
        Protected = 3,
        Internal = 7,
        ProtectedInternal = 15,
        Public = 31
    }
}
