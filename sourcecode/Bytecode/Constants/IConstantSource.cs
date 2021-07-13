using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public interface IConstantSource
    {
        IConstantRef<StringConstant> GetStringConstant(string str);
        //IConstantRef<IStringConstant> ReferenceStringConstant(ulong id);

        IConstantRef<ITypeConstant> GetTypeConstant(Language.IType type, bool defaultBottom = false);
        //IConstantRef<ITypeConstant> ReferenceTypeConstant(ulong id);

        IConstantRef<TypeListConstant> GetTypeListConstant(IEnumerable<Language.IType> type);
        IConstantRef<TypeListConstant> GetTypeListConstant(IEnumerable<Language.ITypeArgument> type);
        //IConstantRef<ITypeListConstant> ReferenceTypeListConstant(ulong id);

        IConstantRef<ClassTypeConstant> GetClassTypeConstant(IParamRef<IInterfaceSpec, Language.ITypeArgument> ct);

        IConstantRef<SuperClassConstant> GetSuperClassConstant(IParamRef<IClassSpec, Language.IType> superclass);

        IConstantRef<ClassConstant> GetClassConstant(IClassSpec cls);
        IConstantRef<IInterfaceConstant> GetInterfaceConstant(IInterfaceSpec cls);
        IConstantRef<StaticMethodConstant> GetStaticMethodConstant(IParameterizedSpecRef<IStaticMethodSpec> staticMethod/*, IEnumerable<Language.IType> typeArgs*/);
        IConstantRef<MethodConstant> GetMethodConstant(IParameterizedSpecRef<IMethodSpec> method/*, IEnumerable<Language.ITypeArgument> typeArgs*/);
    }
}
