using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Nom.Language;
using Nom.TypeChecker;
using System.IO;

namespace Nom.Bytecode
{
    public class LambdaRep
    {
        public IConstantRef<LambdaConstant> LambdaConstant { get; }
        public IConstantRef<TypeParametersConstant> TypeParametersConstant { get; }
        public IConstantRef<ITypeConstant> ReturnTypeConstant { get; }
        public IConstantRef<TypeListConstant> ArgumentsConstant { get; }
        public IConstantRef<TypeParametersConstant> ClosureTypeParametersConstant { get; }
        public IConstantRef<TypeListConstant> ClosureArgumentsConstant { get; }
        public IEnumerable<IInstruction> Instructions { get; }
        public IEnumerable<LambdaFieldRep> Fields { get; }
        public int RegisterCount { get; }
        public LambdaRep(IConstantRef<LambdaConstant> lambda, IConstantRef<TypeParametersConstant> closureTypeParameters, IConstantRef<TypeListConstant> closureArgumentTypes, IEnumerable<LambdaFieldRep> fields, IConstantRef<TypeParametersConstant> typeParameters, IConstantRef<ITypeConstant> returnType, IConstantRef<TypeListConstant> argumentTypes, int registerCount, IEnumerable<IInstruction> instructions)
        {
            LambdaConstant = lambda;
            TypeParametersConstant = typeParameters;
            ReturnTypeConstant = returnType;
            ArgumentsConstant = argumentTypes;
            Instructions = instructions;
            RegisterCount = registerCount;
            ClosureTypeParametersConstant = closureTypeParameters;
            ClosureArgumentsConstant = closureArgumentTypes;
            Fields = fields.ToList();
        }
        public void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.Lambda);
            ws.WriteValue(LambdaConstant.ConstantID);
            ws.WriteValue(ClosureTypeParametersConstant.ConstantID);
            ws.WriteValue(ClosureArgumentsConstant.ConstantID);
            ws.WriteValue(TypeParametersConstant.ConstantID);
            ws.WriteValue(ArgumentsConstant.ConstantID);
            ws.WriteValue(ReturnTypeConstant.ConstantID);
            ws.WriteValue(RegisterCount);
            ws.WriteValue((UInt64)Fields.LongCount());
            foreach(LambdaFieldRep lfr in Fields)
            {
                lfr.WriteByteCode(ws);
            }
            ws.WriteValue(Instructions.Aggregate((UInt64)0, (acc, i) => acc + i.InstructionCount));
            foreach (IInstruction instr in Instructions)
            {
                instr.WriteByteCode(ws);
            }
        }


        public static LambdaRep Read(Language.IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.Lambda)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var lamconst = rcs.ReferenceLambdaConstant(s.ReadULong());
            var ctpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var cargsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var tpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var argsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var rtconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var regcount = s.ReadInt();
            var fieldcount = s.ReadULong();
            List<LambdaFieldRep> fields = new List<LambdaFieldRep>();
            for(ulong i=0;i<fieldcount;i++)
            {
                fields.Add(LambdaFieldRep.Read(s, rcs));
            }
            var instrcount = s.ReadULong();
            List<IInstruction> instructions = new List<IInstruction>();
            for (ulong i = 0; i < instrcount; i++)
            {
                instructions.Add(AInstruction.ReadInstruction(s, rcs));
            }
            var rep = new LambdaRep(lamconst, ctpconst, cargsconst, fields, tpconst, rtconst, argsconst, regcount, instructions);
            return rep;
        }
    }
}
