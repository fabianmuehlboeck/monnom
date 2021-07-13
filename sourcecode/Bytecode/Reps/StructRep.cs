using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using Nom.Language;

namespace Nom.Bytecode
{
    public class StructRep
    {
        public IConstantRef<StructConstant> StructConstant { get; }
        public IConstantRef<TypeParametersConstant> ClosureTypeParametersConstant { get; }
        public IEnumerable<StructFieldRep> Fields { get; }
        IConstantRef<TypeListConstant> InitializerArgTypesConstant { get; }
        public int InitializerRegisterCount { get; }
        public int EndArgRegisterCount { get; }

        private List<MethodDefRep> methods = new List<MethodDefRep>();

        public void AddMethodDef(MethodDefRep mdr)
        {
            methods.Add(mdr);
        }

        public virtual IEnumerable<MethodDeclRep> Methods => methods;
        public StructRep(IConstantRef<StructConstant> structC, IConstantRef<TypeParametersConstant> closureTypeParameters, IEnumerable<StructFieldRep> fields, IConstantRef<TypeListConstant> initializerArgTypes, int initializerRegisterCount, int endArgRegisterCount)
        {
            StructConstant = structC;
            ClosureTypeParametersConstant = closureTypeParameters;
            Fields = fields.ToList();
            InitializerArgTypesConstant = initializerArgTypes;
            InitializerRegisterCount = initializerRegisterCount;
            EndArgRegisterCount = endArgRegisterCount;
        }
        public void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.Struct);
            ws.WriteValue(StructConstant.ConstantID);
            ws.WriteValue(ClosureTypeParametersConstant.ConstantID);
            ws.WriteValue(InitializerRegisterCount);
            ws.WriteValue(EndArgRegisterCount);
            ws.WriteValue(InitializerArgTypesConstant.ConstantID);
            ws.WriteValue((UInt64)Fields.LongCount());
            foreach (StructFieldRep sfr in Fields)
            {
                sfr.WriteByteCode(ws);
            }
            UInt64 methodCount = (ulong)Methods.LongCount();
            ws.WriteValue(methodCount);
            foreach (MethodDefRep mdr in methods)
            {
                mdr.WriteByteCode(ws);
            }
        }


        public static StructRep Read(Language.IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.Lambda)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var structconst = rcs.ReferenceStructConstant(s.ReadULong());
            var ctpconst = rcs.ReferenceTypeParametersConstant(s.ReadULong());
            var iniregcount = s.ReadInt();
            var earegcount = s.ReadInt();
            var iniargsconst = rcs.ReferenceTypeListConstant(s.ReadULong());
            var fieldcount = s.ReadULong();
            List<StructFieldRep> fields = new List<StructFieldRep>();
            for (ulong i = 0; i < fieldcount; i++)
            {
                fields.Add(StructFieldRep.Read(s, rcs));
            }
            var rep = new StructRep(structconst, ctpconst, fields, iniargsconst, iniregcount, earegcount);
            var methodCount = s.ReadULong();
            for (ulong i = 0; i < methodCount; i++)
            {
                rep.AddMethodDef(MethodDefRep.Read(null, s, rcs));
            }
            return rep;
        }
    }
}
