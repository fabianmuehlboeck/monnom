using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public enum BytecodeTopElementType : byte
    {
        None = 0,
        Class = 1,
        Interface = 2,
        StringConstant = 103,
        //IntegerConstant = 104,
        //FloatConstant = 105,
        CTStruct = 110,
        ClassConstant = 111,
        InterfaceConstant = 112,
        CTLambda = 114,
        CTIntersection = 115,
        CTBottom = 117,
        CTDynamicType = 118,
        CTMaybeType = 119,
        ClassTypeConstant = 121,
        CTTypeVar = 122,
        //IntersectionConstant = 125,
        UnionConstant = 125,
        MethodConstant = 131,
        StaticMethodConstant = 132,
        TypeListConstant = 191,
        ClassTypeListConstant = 192,
        //CTClassTypeList = 193,
        CTTypeParameters = 194
    }

    public enum BytecodeInternalElementType : byte
    {
        Field = 201,
        StaticMethod = 202,
        Method = 203,
        Constructor = 204,
        Lambda = 205,
        Struct = 206
    };
}
