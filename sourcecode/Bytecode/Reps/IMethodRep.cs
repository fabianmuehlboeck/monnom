using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public interface IMethodRep : IMethodSpec
    {
        IConstantRef<StringConstant> NameConstant
        {
            get;
        }
        IConstantRef<ITypeConstant> ReturnTypeConstant
        {
            get;
        }

        IConstantRef<TypeListConstant> ArgumentTypesConstant
        {
            get;
        }
    }
}
