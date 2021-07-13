using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Nom.Bytecode
{
    public struct TypeParameterEntry
    {
        //public readonly Variance Variance;
        public readonly IConstantRef<ITypeConstant> LowerBound;
        public readonly IConstantRef<ITypeConstant> UpperBound;

        public TypeParameterEntry(/*Variance variance,*/ IConstantRef<ITypeConstant> lowerBound, IConstantRef<ITypeConstant> upperBound)
        {
            //Variance = variance;
            LowerBound = lowerBound;
            UpperBound = upperBound;
        }
    }
    public class TypeParametersConstant : AConstant
    {
        public IEnumerable<TypeParameterEntry> Entries { get; }

        public override IEnumerable<IConstant> Dependencies => Entries.Select(e => e.LowerBound.Constant).Concat(Entries.Select(e => e.UpperBound.Constant));

        public TypeParametersConstant(ulong id, IEnumerable<TypeParameterEntry> entries) : base(ConstantType.CTTypeParameters, id)
        {
            Entries = entries.ToList();
        }

        public override void EmitBody(Stream ws)
        {
            ws.WriteValue((UInt64)Entries.LongCount());
            foreach(var entry in Entries)
            {
                //ws.WriteByte((byte)entry.Variance);
                ws.WriteValue(entry.LowerBound.ConstantID);
                ws.WriteValue(entry.UpperBound.ConstantID);
            }
        }
        public static TypeParametersConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            List<TypeParameterEntry> entries = new List<TypeParameterEntry>();
            ulong count = s.ReadULong();
            for (ulong i = 0; i < count; i++)
            {
                ulong lower = s.ReadULong();
                ulong upper = s.ReadULong();
                entries.Add(new TypeParameterEntry(rcs.ReferenceTypeConstant(lower), rcs.ReferenceTypeConstant(upper)));
            }
            return new TypeParametersConstant(id, entries);
        }
    }
}
