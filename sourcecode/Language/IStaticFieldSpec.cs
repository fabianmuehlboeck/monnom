using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IStaticFieldSpec : ISpec, IMember
    {
        bool IsReadonly
        {
            get;
        }
        bool IsVolatile
        {
            get;
        }
        String Name
        {
            get;
        }
        IType Type
        {
            get;
        }
    }
}
