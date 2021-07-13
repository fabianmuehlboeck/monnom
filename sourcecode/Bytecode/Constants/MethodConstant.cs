using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class MethodConstant : AConstant
    {
        public IConstantRef<ClassTypeConstant> ReceiverTypeConstant { get; }
        public IConstantRef<StringConstant> MethodNameConstant { get; }
        public IConstantRef<TypeListConstant> TypeArgumentsConstant { get; }
        public IConstantRef<TypeListConstant> ArgumentTypesConstant { get; }
        public MethodConstant(ulong id, IConstantRef<ClassTypeConstant> receiverType, IConstantRef<StringConstant> methodName, IConstantRef<TypeListConstant> typeArgs, IConstantRef<TypeListConstant> argTypes) : base(ConstantType.CTMethod, id)
        {
            ReceiverTypeConstant = receiverType;
            MethodNameConstant = methodName;
            TypeArgumentsConstant = typeArgs;
            ArgumentTypesConstant = argTypes;
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                yield return ReceiverTypeConstant.Constant;
                yield return MethodNameConstant.Constant;
                yield return TypeArgumentsConstant.Constant;
                yield break;
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue(ReceiverTypeConstant.ConstantID);
            s.WriteValue(MethodNameConstant.ConstantID);
            s.WriteValue(TypeArgumentsConstant.ConstantID);
            s.WriteValue(ArgumentTypesConstant.ConstantID);
        }
        public static MethodConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong receiverType = s.ReadULong();
            ulong methodName = s.ReadULong();
            ulong typeArgs = s.ReadULong();
            ulong argTypes = s.ReadULong();
            return new MethodConstant(id, rcs.ReferenceClassTypeConstant(receiverType), rcs.ReferenceStringConstant(methodName), rcs.ReferenceTypeListConstant(typeArgs), rcs.ReferenceTypeListConstant(argTypes));
        }
    }
}
