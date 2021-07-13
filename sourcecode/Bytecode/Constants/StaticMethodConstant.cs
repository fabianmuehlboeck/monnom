using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class StaticMethodConstant : AConstant
    {
        public IConstantRef<ClassConstant> ClassConstant {get;}
        public IConstantRef<StringConstant> MethodNameConstant { get; }
        public IConstantRef<TypeListConstant> TypeArgumentsConstant { get; }
        public IConstantRef<TypeListConstant> ArgumentTypesConstant { get; }
        public StaticMethodConstant(ulong id, IConstantRef<ClassConstant> cls, IConstantRef<StringConstant> methodName, IConstantRef<TypeListConstant> typeArgs, IConstantRef<TypeListConstant> argTypes) : base(ConstantType.CTStaticMethod, id)
        {
            ClassConstant = cls;
            MethodNameConstant = methodName;
            TypeArgumentsConstant = typeArgs;
            ArgumentTypesConstant = argTypes;
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                yield return ClassConstant.Constant;
                yield return MethodNameConstant.Constant;
                yield return TypeArgumentsConstant.Constant;
                yield break;
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue(ClassConstant.ConstantID);
            s.WriteValue(MethodNameConstant.ConstantID);
            s.WriteValue(TypeArgumentsConstant.ConstantID);
            s.WriteValue(ArgumentTypesConstant.ConstantID);
        }
        public static StaticMethodConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong cls = s.ReadULong();
            ulong methodName = s.ReadULong();
            ulong typeArgs = s.ReadULong();
            ulong argTypes = s.ReadULong();
            return new StaticMethodConstant(id, rcs.ReferenceClassConstant(cls), rcs.ReferenceStringConstant(methodName), rcs.ReferenceTypeListConstant(typeArgs), rcs.ReferenceTypeListConstant(argTypes));
        }
    }
}
