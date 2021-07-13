using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public enum ConstantType : byte
    {
        CTString = 103,
        //CTInteger = 104,
        //CTFloat = 105,
        CTStruct= 110,
        CTClass = 111,
        CTInterface = 112,
        //CTTypeParameter = 113,
        CTLambda = 114,
        CTIntersection = 115,
        //CTUnion = 116,
        CTBottom = 117,
        CTDynamicType = 118,
        CTMaybeType = 119,
        CTClassType = 121,
        CTTypeVar = 122,
        CTMethod = 131,
        CTStaticMethod = 132,
        CTSuperClass = 141,
        //CTSuperInterface = 142,
        CTTypeList = 191,
        CTSuperInterfaceList = 192,
        //CTClassTypeList = 193,
        CTTypeParameters = 194
    }

    public interface IConstant
    {
        ulong ConstantID { get; }
        ConstantType Type
        {
            get;
        }

        IEnumerable<IConstant> Dependencies
        {
            get;
        }

        void Emit(Stream ws);
    }
}
