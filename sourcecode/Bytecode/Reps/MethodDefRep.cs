using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.IO;

namespace Nom.Bytecode
{
    public class MethodDefRep : MethodDeclRep
    {
        public MethodDefRep(IConstantRef<StringConstant> name, IConstantRef<TypeParametersConstant> typeParameters, IConstantRef<ITypeConstant> returnType, IConstantRef<TypeListConstant> argumentTypes, Visibility visibility, bool isFinal, int registerCount, IEnumerable<IInstruction> instructions) : base(name, typeParameters, returnType, argumentTypes, visibility, isFinal)
        {
            Instructions = instructions;
            RegisterCount = registerCount;
        }

        public int RegisterCount { get; }
        public IEnumerable<IInstruction> Instructions { get; }

        public override void WriteByteCode(Stream ws)
        {
            base.WriteByteCode(ws);
        }
        public override void WriteByteCodeBody(Stream ws)
        {
            ws.WriteValue(RegisterCount);
            ws.WriteValue(Instructions.Aggregate((UInt64)0, (acc, i) => acc + i.InstructionCount));
            foreach (IInstruction instr in Instructions)
            {
                instr.WriteByteCode(ws);
            }
        }
        new public static MethodDefRep Read(Language.IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if(tag!= (byte)BytecodeInternalElementType.Method)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var tpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var rtconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var argsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var isfinal = s.ReadActualByte() == 1;
            var regcount = s.ReadInt();
            var instrcount = s.ReadULong();
            List<IInstruction> instructions = new List<IInstruction>();
            for(ulong i=0; i<instrcount;i++)
            {
                instructions.Add(AInstruction.ReadInstruction(s, rcs));
            }
            //TODO: actually put visibility in bytecode here
            return new MethodDefRep(nameconst, tpconst, rtconst, argsconst, Visibility.Public, isfinal, regcount, instructions);
        }
    }
}
