using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

namespace Nom.Bytecode
{
    public class StructFieldRep
    {
        public IConstantRef<StringConstant> FieldNameConstant { get; }
        public IConstantRef<ITypeConstant> TypeConstant { get; }
        public bool IsReadOnly { get; }
        public int ValueRegister { get; }
        public IEnumerable<IInstruction> InitializerInstructions { get; }

        public StructFieldRep(IConstantRef<StringConstant> fieldName, IConstantRef<ITypeConstant> type, bool isReadOnly, int valueRegister, IEnumerable<IInstruction> initializerInstructions)
        {
            FieldNameConstant = fieldName;
            TypeConstant = type;
            IsReadOnly = isReadOnly;
            ValueRegister = valueRegister;
            InitializerInstructions = initializerInstructions.ToList();
        }

        public void WriteByteCode(Stream ws)
        {
            ws.WriteValue(FieldNameConstant.ConstantID);
            ws.WriteValue(TypeConstant.ConstantID);
            ws.WriteByte(IsReadOnly?(byte)1:(byte)0);
            ws.WriteValue(ValueRegister);
            ws.WriteValue(InitializerInstructions.Count());
            foreach(IInstruction instr in InitializerInstructions)
            {
                instr.WriteByteCode(ws);
            }
        }

        public static StructFieldRep Read(Stream s, IReadConstantSource rcs)
        {
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var typeconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var isreadonly = s.ReadActualByte() == 1;
            var valueReg = s.ReadInt();
            var instrcount = s.ReadInt();
            List<IInstruction> instructions = new List<IInstruction>();
            for(int i=0;i<instrcount;i++)
            {
                instructions.Add(AInstruction.ReadInstruction(s, rcs)); ;
            }
            return new StructFieldRep(nameconst, typeconst, isreadonly, valueReg, instructions);
        }
    }
}
