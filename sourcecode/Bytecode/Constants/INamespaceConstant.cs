using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface INamespaceConstant : IConstant
    {
        //IOptional<IConstantRef<INamespaceConstant>> ParentConstant
        //{
        //    get;
        //}
        IConstantRef<StringConstant> NameConstant
        {
            get;
        }
        String QualifiedName
        {
            get;
        }
    }
}
