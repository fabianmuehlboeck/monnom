using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.IO;

namespace Nom.Bytecode
{
    public class MethodDeclRep : TypeChecker.AParameterized, IMethodRep
    {
        public MethodDeclRep(IConstantRef<StringConstant> name, IConstantRef<TypeParametersConstant> typeParameters, IConstantRef<ITypeConstant> returnType, IConstantRef<TypeListConstant> argumentTypes, Visibility visibility, bool isFinal)
        {
            NameConstant = name;
            TypeParametersConstant = typeParameters;
            ReturnTypeConstant = returnType;
            ArgumentTypesConstant = argumentTypes;
            IsFinal = isFinal;
        }

        public bool IsFinal { get; }

        public IConstantRef<StringConstant> NameConstant
        {
            get;
        }
        public IConstantRef<TypeParametersConstant> TypeParametersConstant
        {
            get;
        }
        public string Name
        {
            get
            {
                return NameConstant.Constant.Value;
            }
        }
        
        public IConstantRef<ITypeConstant> ReturnTypeConstant
        {
            get;
        }

        public IConstantRef<TypeListConstant> ArgumentTypesConstant
        {
            get;
        }

        public bool IsVirtual => throw new NotImplementedException();

        public IParametersSpec Parameters => throw new NotImplementedException();

        public IType ReturnType => throw new NotImplementedException();

        protected override IOptional<IParameterizedSpec> ParamParent => throw new NotImplementedException();

        public override ITypeParametersSpec TypeParameters => throw new NotImplementedException();

        public INamespaceSpec Container => throw new NotImplementedException();

        public Visibility Visibility => throw new NotImplementedException();

        //public int OverallTypeParameterCount => throw new NotImplementedException();

        public bool Overrides(IMethodSpec other, ITypeEnvironment<IType> substitutions)
        {
            throw new NotImplementedException();
        }

        public virtual void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.Method);
            ws.WriteValue(NameConstant.ConstantID);
            ws.WriteValue(TypeParametersConstant.ConstantID);
            ws.WriteValue(ReturnTypeConstant.ConstantID);
            ws.WriteValue(ArgumentTypesConstant.ConstantID);
            ws.WriteByte((byte)(IsFinal ? 1 : 0));
            WriteByteCodeBody(ws);
        }

        public virtual void WriteByteCodeBody(Stream ws)
        {
            ws.WriteValue((int)0);
            ws.WriteValue((UInt64)0);
        }

        public static MethodDeclRep Read(IInterfaceSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.Method)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var tpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var rtconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var argsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var isfinal = s.ReadActualByte() == 1;
            var intnull = s.ReadInt();
            var longnull = s.ReadULong();
            if(intnull!=0 || longnull!=0)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            //TODO: actually put visibility in bytecode here
            return new MethodDeclRep(nameconst, tpconst, rtconst, argsconst, Visibility.Public, isfinal);
        }
    }
}
