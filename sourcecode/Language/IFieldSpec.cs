using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Language
{
    public interface IFieldSpec : ISpec, IMember
    {
        new IClassSpec Container { get; }
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
