using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface INamedConstant : IConstant
    {
        IConstantRef<StringConstant> LibraryNameConstant
        {
            get;
        }

        IConstantRef<StringConstant> NameConstant
        {
            get;
        }

        //UInt64 TypeArgumentCount
        //{
        //    get;
        //    set;
        //}
    }
}
