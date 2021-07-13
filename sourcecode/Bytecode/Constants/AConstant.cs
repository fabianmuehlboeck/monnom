using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public abstract class AConstant : IConstant
    {
        public ulong ConstantID { get; }

        public AConstant(ConstantType type, ulong id)
        {
            this.Type = type;
            ConstantID = id;
        }
        

        public ConstantType Type
        {
            get;
            private set;
        }

        public abstract IEnumerable<IConstant> Dependencies
        {
            get;
        }

        public void Emit(Stream ws)
        {
            ws.WriteByte((byte)Type);
            ws.WriteValue(ConstantID);
            EmitBody(ws);
        }

        public static IConstant ReadConstant(Stream s, IReadConstantSource rcs)
        {
            int tag = s.ReadByte();
            if(tag<0)
            {
                throw new InvalidDataException();
            }
            byte type = (byte)tag;
            ulong id = s.ReadULong();
            switch ((ConstantType)type)
            {
                case ConstantType.CTBottom:
                    return BottomTypeConstant.Read(s, id, rcs);
                case ConstantType.CTClass:
                    return ClassConstant.Read(s, id, rcs);
                case ConstantType.CTClassType:
                    return ClassTypeConstant.Read(s, id, rcs);
                case ConstantType.CTDynamicType:
                    return DynamicTypeConstant.Read(s, id, rcs);
                case ConstantType.CTInterface:
                    return InterfaceConstant.Read(s, id, rcs);
                case ConstantType.CTIntersection:
                    return IntersectionTypeConstant.Read(s, id, rcs);
                case ConstantType.CTLambda:
                    return LambdaConstant.Read(s, id, rcs);
                case ConstantType.CTMethod:
                    return MethodConstant.Read(s, id, rcs);
                case ConstantType.CTStaticMethod:
                    return StaticMethodConstant.Read(s, id, rcs);
                case ConstantType.CTString:
                    return StringConstant.Read(s, id, rcs);
                case ConstantType.CTStruct:
                    return StructConstant.Read(s, id, rcs);
                case ConstantType.CTSuperClass:
                    return SuperClassConstant.Read(s, id, rcs);
                case ConstantType.CTSuperInterfaceList:
                    return SuperInterfacesConstant.Read(s, id, rcs);
                case ConstantType.CTTypeList:
                    return TypeListConstant.Read(s, id, rcs);
                case ConstantType.CTTypeParameters:
                    return TypeParametersConstant.Read(s, id, rcs);
                case ConstantType.CTTypeVar:
                    return TypeVariableConstant.Read(s, id, rcs);
                case ConstantType.CTMaybeType:
                    return MaybeTypeConstant.Read(s, id, rcs);
                default:
                    throw new InvalidDataException();
            }
        }

        public abstract void EmitBody(Stream s);
    }
}
