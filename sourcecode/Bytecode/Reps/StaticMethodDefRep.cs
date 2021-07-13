using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Nom.Language;
using System.Linq;
using Nom.Language.SpecExtensions;

namespace Nom.Bytecode
{
    public class StaticMethodDefRep : TypeChecker.AParameterized, IStaticMethodSpec
    {
        public StaticMethodDefRep(IConstantRef<IStringConstant> name, IConstantRef<ITypeConstant> returnType, IConstantRef<TypeParametersConstant> typeParamConstraints, IConstantRef<TypeListConstant> parameters, Visibility visibility, IEnumerable<IInstruction> instructions, int regcount)
        {
            NameConstant = name;
            ReturnTypeConstant = returnType;
            TypeParametersConstant = typeParamConstraints;
            ParametersConstant = parameters;
            Visibility = Visibility;
            Instructions = instructions;
            RegisterCount = regcount;
        }
        INamespaceSpec IMember.Container => Container;
        public IClassSpec Container { get; }
        public IConstantRef<IStringConstant> NameConstant { get; }
        public IConstantRef<ITypeConstant> ReturnTypeConstant { get; }
        public IConstantRef<TypeParametersConstant> TypeParametersConstant { get; }
        public IConstantRef<TypeListConstant> ParametersConstant { get; }
        public string Name => NameConstant.Constant.Value;

        public int RegisterCount { get; }

        public override ITypeParametersSpec TypeParameters => throw new NotImplementedException();

        public IParametersSpec Parameters => throw new NotImplementedException();

        public IType ReturnType => throw new NotImplementedException();

        public Visibility Visibility { get; }

        public IEnumerable<IInstruction> Instructions { get; }

        //public IOptional<IParameterizedSpecRef<IParameterizedSpec>> ParameterizedParent => throw new NotImplementedException();

        //public int OverallTypeParameterCount => throw new NotImplementedException();

        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        public void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.StaticMethod);
            ws.WriteValue(NameConstant.ConstantID);
            ws.WriteValue(TypeParametersConstant.ConstantID);
            ws.WriteValue(ReturnTypeConstant.ConstantID);
            ws.WriteValue(ParametersConstant.ConstantID);
            ws.WriteValue(RegisterCount);
            ws.WriteValue(Instructions.Aggregate((UInt64)0, (acc,i)=>acc+i.InstructionCount));
            foreach(IInstruction i in Instructions)
            {
                i.WriteByteCode(ws);
            }
        }

        public static StaticMethodDefRep Read(Language.IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.StaticMethod)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var tpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var rtconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var argsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var regcount = s.ReadInt();
            var instrcount = s.ReadULong();
            List<IInstruction> instructions = new List<IInstruction>();
            for (ulong i = 0; i < instrcount; i++)
            {
                instructions.Add(AInstruction.ReadInstruction(s, rcs));
            }
            //TODO: actually put visibility in bytecode here
            return new StaticMethodDefRep(nameconst, rtconst, tpconst, argsconst, Visibility.Public, instructions, regcount);
        }

        //public IParameterizedSpecRef<IParameterizedSpec> GetAsRef()
        //{
        //    return new ParameterizedSpecRef<IParameterizedSpec>(this, TypeParameters.ToTypeEnvironment());
        //}
    }
}
