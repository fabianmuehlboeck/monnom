using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class BytecodeReader : IReadConstantSource
    {
        public BytecodeReader() { }

        public Version Version { get; }

        private class ConstantRef<T> : IConstantRef<T> where T : IConstant
        {
            private ulong id;
            private BytecodeReader parent;
            public ConstantRef(ulong id, BytecodeReader parent)
            {
                this.id = id;
                this.parent = parent;
            }

            public T Constant => (T)parent.Constants[id];

            public ulong ConstantID => id;
        }

        public IConstantRef<ClassConstant> ReferenceClassConstant(ulong id)
        {
            return new ConstantRef<ClassConstant>(id, this);
        }

        public IConstantRef<ClassTypeConstant> ReferenceClassTypeConstant(ulong id)
        {
            return new ConstantRef<ClassTypeConstant>(id, this);
        }

        public IConstantRef<InterfaceConstant> ReferenceInterfaceConstant(ulong id)
        {
            return new ConstantRef<InterfaceConstant>(id, this);
        }

        public IConstantRef<MethodConstant> ReferenceMethodConstant(ulong id)
        {
            return new ConstantRef<MethodConstant>(id, this);
        }

        public IConstantRef<INamedConstant> ReferenceNamedConstant(ulong id)
        {
            return new ConstantRef<INamedConstant>(id, this);
        }

        public IConstantRef<StaticMethodConstant> ReferenceStaticMethodConstant(ulong id)
        {
            return new ConstantRef<StaticMethodConstant>(id, this);
        }

        public IConstantRef<StringConstant> ReferenceStringConstant(ulong id)
        {
            return new ConstantRef<StringConstant>(id, this);
        }

        public IConstantRef<SuperClassConstant> ReferenceSuperClassConstant(ulong id)
        {
            return new ConstantRef<SuperClassConstant>(id, this);
        }

        public IConstantRef<ITypeConstant> ReferenceTypeConstant(ulong id)
        {
            return new ConstantRef<ITypeConstant>(id, this);
        }

        public IConstantRef<TypeListConstant> ReferenceTypeListConstant(ulong id)
        {
            return new ConstantRef<TypeListConstant>(id, this);
        }

        private Dictionary<ulong, IConstant> Constants = new Dictionary<ulong, IConstant>();

        private List<InterfaceRep> interfaces = new List<InterfaceRep>();
        private List<ClassRep> classes = new List<ClassRep>();
        public IEnumerable<InterfaceRep> Interfaces => interfaces;
        public IEnumerable<ClassRep> Classes => classes;

        public void ReadBytecodeFile(FileInfo fi)
        {
            if (!fi.Exists)
            {
                throw new NomBytecodeException("File not found: " + fi.FullName);
            }
            using (FileStream s = fi.OpenRead())
            {
                uint bytecode_version = s.ReadUInt();
                if (bytecode_version > BytecodeUnit.BYTECODE_VERSION)
                {
                    throw new NomBytecodeException("Bytecode version of file " + fi.FullName + " is too new. Please update!");
                }
                while (s.Position < s.Length)
                {
                    BytecodeTopElementType nextType = (BytecodeTopElementType)s.ReadByte();
                    switch (nextType)
                    {
                        case BytecodeTopElementType.Class:
                            classes.Add(ClassRep.Read(s, this));
                            break;
                        case BytecodeTopElementType.Interface:
                            interfaces.Add(InterfaceRep.Read(s, this));
                            break;
                        case BytecodeTopElementType.StringConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, StringConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.ClassConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, ClassConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.InterfaceConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, InterfaceConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.ClassTypeConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, ClassTypeConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.MethodConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, MethodConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.StaticMethodConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, StaticMethodConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.TypeListConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, TypeListConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.ClassTypeListConstant:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, SuperInterfacesConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTStruct:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, StructConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTLambda:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, LambdaConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTIntersection:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, IntersectionTypeConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTBottom:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, BottomTypeConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTDynamicType:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, DynamicTypeConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTMaybeType:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, MaybeTypeConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.CTTypeVar:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, TypeVariableConstant.Read(s, id, this));
                                break;
                            }

                        case BytecodeTopElementType.CTTypeParameters:
                            {
                                ulong id = s.ReadULong();
                                Constants.Add(id, TypeParametersConstant.Read(s, id, this));
                                break;
                            }
                        case BytecodeTopElementType.None:
                        default:
                            throw new NomBytecodeException("Invalid Bytecode Element type!");
                    }
                }
            }
        }

        public IConstantRef<TypeParametersConstant> ReferenceTypeParametersConstant(ulong id)
        {
            return new ConstantRef<TypeParametersConstant>(id, this);
        }

        public IConstantRef<SuperInterfacesConstant> ReferenceSuperInterfacesConstant(ulong id)
        {
            return new ConstantRef<SuperInterfacesConstant>(id, this);
        }

        public IConstantRef<LambdaConstant> ReferenceLambdaConstant(ulong id)
        {
            return new ConstantRef<LambdaConstant>(id, this);
        }

        public IConstantRef<StructConstant> ReferenceStructConstant(ulong id)
        {
            return new ConstantRef<StructConstant>(id, this);
        }

        public IConstantRef<IConstant> ReferenceConstant(ulong id)
        {
            return new ConstantRef<IConstant>(id, this);
        }
    }
}
