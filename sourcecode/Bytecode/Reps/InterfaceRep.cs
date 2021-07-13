using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using System.IO;
using Nom.TypeChecker;

namespace Nom.Bytecode
{
    public class InterfaceRep : AParameterized, IInterfaceSpec
    {
        public IConstantRef<StringConstant> NameConstant
        {
            get;
        }

        public IConstantRef<TypeParametersConstant> TypeParameterConstraintsConstant
        {
            get;
        }

        private List<MethodDeclRep> methodDecls = new List<MethodDeclRep>();

        public void AddMethodDecl(MethodDeclRep mdr)
        {
            if (!methodDecls.Contains(mdr))
            {
                methodDecls.Add(mdr);
            }
        }

        public void RemoveMethodDecl(MethodDeclRep mdr)
        {
            while(methodDecls.Contains(mdr))
            {
                methodDecls.Remove(mdr);
            }
        }

        public IEnumerable<MethodDeclRep> MethodDeclarations
        {
            get
            {
                return methodDecls.ToList();
            }
        }

        public IConstantRef<SuperInterfacesConstant> SuperInterfacesConstant
        {
            get;
            set;
        }

        public UInt64 TypeArgumentCount
        {
            get;
            set;
        } = 0;

        public bool IsShape { get; }
        public bool IsExpando { get; }

        public Visibility Visibility { get; }

        public ILibrary Library { get; }

        public IEnumerable<IParamRef<IInterfaceSpec, IType>> Implements => throw new NotImplementedException(); 

        public virtual IEnumerable<MethodDeclRep> Methods => methodDecls;

        IEnumerable<IMethodSpec> IInterfaceSpec.Methods => Methods;

        public IEnumerable<INamespaceSpec> Children => interfaces;

        public IOptional<INamespaceSpec> ParentNamespace => throw new NotImplementedException();
        protected override IOptional<IParameterizedSpec> ParamParent => ParentNamespace;

        public string Name => NameConstant.Constant.Value;

        private class TypeParamSpec : ITypeParameterSpec
        {
            public TypeParamSpec(IParameterizedSpec parent, int index, string name)
            {
                Parent = parent;
                Index = index;
                Name = name;
            }
            public IParameterizedSpec Parent { get; set; }

            public int Index { get; }

            public string Name { get; }

            public IType UpperBound { get; private set; }

            public IType LowerBound { get; private set; }

            public void AdjustBounds(IType upperBound = null, IType lowerBound = null)
            {
                UpperBound = upperBound;
                LowerBound = lowerBound;
            }
        }
        private ITypeParametersSpec tparams = null;
        public override ITypeParametersSpec TypeParameters
        {
            get
            {
                if (tparams == null)
                {
                    List<TypeParamSpec> paramList = new List<TypeParamSpec>();
                    int count = 0;
                    foreach(var entry in TypeParameterConstraintsConstant.Constant.Entries)
                    {
                        paramList.Add(new TypeParamSpec(this, count, "T" + count.ToString()));
                        count++;
                    }
                    tparams = new TypeParametersSpec(paramList);
                    foreach(var tpp in paramList.Zip(TypeParameterConstraintsConstant.Constant.Entries, (x,y)=>(x,y)))
                    {
                        tpp.x.AdjustBounds(tpp.y.UpperBound.Constant.Value ?? Language.TopType.Instance, tpp.y.LowerBound.Constant.Value ?? Language.BotType.Instance);
                    }
                }
                return tparams;
            }
        }

        public IEnumerable<INamespaceSpec> Namespaces
        {
            get
            {
                yield break;
            }
        }

        public IEnumerable<IInterfaceSpec> Interfaces => interfaces;

        public virtual IEnumerable<IClassSpec> Classes
        {
            get
            {
                yield break;
            }
        }

        public IEnumerable<INamespaceSpec> PublicChildren => interfaces.Where(ifc => ifc.Visibility == Visibility.Public);

        public IEnumerable<INamespaceSpec> ProtectedChildren => interfaces.Where(ifc => ifc.Visibility == Visibility.Protected);

        public string FullQualifiedName => Name;

        //public static InterfaceRep Create(String name)
        //{
        //    return new InterfaceRep(new StringConstant(name));
        //}

        //public static InterfaceRep Create(IStringConstant nameConstant)
        //{
        //    return new InterfaceRep(nameConstant);
        //}

        public virtual Ret Visit<Arg, Ret>(INamespaceSpecVisitor<Arg, Ret> visitor, Arg arg = default(Arg))
        {
            return visitor.VisitInterfaceSpec(this, arg);
        }

        public InterfaceRep(IConstantRef<StringConstant> nameConstant, IConstantRef<TypeParametersConstant> typeArgConstraints, IConstantRef<SuperInterfacesConstant> superInterfaces, bool isShape, Visibility visibility, AssemblyUnit au)
        {
            this.NameConstant = nameConstant;
            Visibility = visibility;
            IsShape = isShape;
            TypeParameterConstraintsConstant = typeArgConstraints;
            Library = au;
            SuperInterfacesConstant = superInterfaces;
        }

        protected virtual IEnumerable<bool> GetFlagBools()
        {
            yield return IsShape;
        }
        protected virtual BytecodeTopElementType BytecodeType => BytecodeTopElementType.Interface;

        //public int OverallTypeParameterCount => throw new NotImplementedException();

        protected virtual void EmitHeader(Stream ws)
        {
            ws.WriteByte((byte)BytecodeType);
            ws.WriteValue(NameConstant.ConstantID);
            ws.WriteValue(TypeParameterConstraintsConstant.ConstantID);
            ws.WriteByte((byte)Visibility);
            ws.WriteBytes(GetFlagBools().Compress());
            ws.WriteValue(SuperInterfacesConstant.ConstantID);
        }
        protected virtual void EmitBody(Stream ws)
        {
            UInt64 methodCount = (ulong)Methods.LongCount();
            ws.WriteValue(methodCount);
            foreach (MethodDeclRep mdr in Methods)
            {
                mdr.WriteByteCode(ws);
            }
        }

        public void Emit(Stream ws)
        {
            EmitHeader(ws);
            EmitBody(ws);
            EmitChildren(ws);
        }

        protected virtual void EmitChildren(Stream ws)
        {
            foreach(InterfaceRep ir in interfaces)
            {
                ir.Emit(ws);
            }
        }

        public static InterfaceRep Read(Stream ws, IReadConstantSource rcs)
        {
            var nameConstant = rcs.ReferenceStringConstant(ws.ReadULong());
            var tpconstraints = rcs.ReferenceTypeParametersConstant(ws.ReadULong());
            Visibility visibility = (Visibility)ws.ReadByte();
            var isShape =  ws.Decompress(1).First();
            var superInterfaces = rcs.ReferenceSuperInterfacesConstant(ws.ReadULong());
            var iface = new InterfaceRep(nameConstant, tpconstraints, superInterfaces, isShape, visibility, null);
            UInt64 methodCount = ws.ReadULong();
            for(UInt64 i=0;i<methodCount;i++)
            {
                iface.AddMethodDecl(MethodDeclRep.Read(iface, ws, rcs));
            }
            return iface;
        }

        public INamedType Instantiate(ITypeEnvironment<ITypeArgument> args)
        {
            throw new NotImplementedException();
        }

        private List<InterfaceRep> interfaces = new List<InterfaceRep>();
        public void AddInterface(InterfaceRep ir)
        {
            interfaces.Add(ir);
        }
    }
}
