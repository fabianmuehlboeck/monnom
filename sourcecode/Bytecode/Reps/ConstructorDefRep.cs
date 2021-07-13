using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.IO;
using Nom.Language;

namespace Nom.Bytecode
{
    public class ConstructorDefRep : TypeChecker.AParameterized, IConstructorSpec
    {
        public ConstructorDefRep(IConstantRef<TypeListConstant> parameters, Visibility visibility, IEnumerable<IInstruction> preinstructions, IEnumerable<int> superCallArgsRegs, IEnumerable<IInstruction> postinstructions, int regcount)
        {
            ParametersConstant = parameters;
            Visibility = Visibility;
            PreInstructions = preinstructions;
            PostInstructions = postinstructions;
            SuperConstructorArgs = superCallArgsRegs;
            RegisterCount = regcount;
        }
        public int RegisterCount { get; }
        public IClassSpec Container { get; }
        public IConstantRef<TypeListConstant> ParametersConstant { get; }
        public readonly IEnumerable<IInstruction> PreInstructions;
        public readonly IEnumerable<IInstruction> PostInstructions;
        public readonly IEnumerable<int> SuperConstructorArgs;

        public override ITypeParametersSpec TypeParameters => throw new NotImplementedException();

        public IParametersSpec Parameters => throw new NotImplementedException();

        public IType ReturnType => throw new NotImplementedException();

        public Visibility Visibility { get; }

        INamespaceSpec IMember.Container => Container;


        protected override IOptional<IParameterizedSpec> ParamParent => Container.InjectOptional();

        //public int OverallTypeParameterCount => ParameterizedParent.Extract(p => p.OverallTypeParameterCount);

        public void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.Constructor);
            ws.WriteValue(ParametersConstant.ConstantID);
            ws.WriteValue(RegisterCount);
            ws.WriteValue(PreInstructions.Aggregate((UInt64)0, (acc, i) => acc + i.InstructionCount));
            ws.WriteValue((UInt64)SuperConstructorArgs.LongCount());
            ws.WriteValue(PostInstructions.Aggregate((UInt64)0, (acc, i) => acc + i.InstructionCount));
            foreach (IInstruction i in PreInstructions)
            {
                i.WriteByteCode(ws);
            }
            foreach (int regIndex in SuperConstructorArgs)
            {
                ws.WriteValue(regIndex);
            }
            foreach (IInstruction i in PostInstructions)
            {
                i.WriteByteCode(ws);
            }
        }
        public static ConstructorDefRep Read(Language.IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.Constructor)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var argsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var regcount = s.ReadInt();
            var preinstcount = s.ReadULong();
            var scacount = s.ReadULong();
            var postinstcount = s.ReadULong();
            List<IInstruction> preinstructions = new List<IInstruction>();
            List<IInstruction> postinstructions = new List<IInstruction>();
            List<int> superConstructorArgs = new List<int>();
            for (ulong i = 0; i < preinstcount; i++)
            {
                preinstructions.Add(AInstruction.ReadInstruction(s, rcs));
            }
            for (ulong i = 0; i < scacount; i++)
            {
                superConstructorArgs.Add(s.ReadInt());
            }
            for (ulong i = 0; i < postinstcount; i++)
            {
                postinstructions.Add(AInstruction.ReadInstruction(s, rcs));
            }
            //TODO: actually put visibility in bytecode here
            return new ConstructorDefRep(argsconst, Visibility.Public, preinstructions, superConstructorArgs, postinstructions, regcount);
        }
    }
}
