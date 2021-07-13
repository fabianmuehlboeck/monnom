using System;
using System.Collections.Generic;
using System.Text;

namespace Nom.Bytecode
{
    public interface IReadConstantSource
    {
        IConstantRef<StringConstant> ReferenceStringConstant(ulong id);

        IConstantRef<ITypeConstant> ReferenceTypeConstant(ulong id);
        IConstantRef<INamedConstant> ReferenceNamedConstant(ulong id);

        IConstantRef<TypeListConstant> ReferenceTypeListConstant(ulong id);

        IConstantRef<ClassTypeConstant> ReferenceClassTypeConstant(ulong id);

        IConstantRef<SuperClassConstant> ReferenceSuperClassConstant(ulong id);

        IConstantRef<ClassConstant> ReferenceClassConstant(ulong id);
        IConstantRef<InterfaceConstant> ReferenceInterfaceConstant(ulong id);
        IConstantRef<StaticMethodConstant> ReferenceStaticMethodConstant(ulong id);
        IConstantRef<MethodConstant> ReferenceMethodConstant(ulong id);
        IConstantRef<TypeParametersConstant> ReferenceTypeParametersConstant(ulong id);
        IConstantRef<SuperInterfacesConstant> ReferenceSuperInterfacesConstant(ulong id);
        IConstantRef<LambdaConstant> ReferenceLambdaConstant(ulong id);
        IConstantRef<StructConstant> ReferenceStructConstant(ulong id);
        IConstantRef<IConstant> ReferenceConstant(ulong id);
        Version Version { get; }
    }
}
