using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Nom.Bytecode
{
    public class SuperInterfacesConstant : AConstant
    {
        public IEnumerable<(IConstantRef<IInterfaceConstant>, IConstantRef<TypeListConstant>)> Entries { get; }
        public SuperInterfacesConstant(ulong id, IEnumerable<(IConstantRef<IInterfaceConstant>, IConstantRef<TypeListConstant>)> entries) : base(ConstantType.CTSuperInterfaceList, id)
        {
            Entries = entries.ToList();
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                foreach (var entry in Entries)
                {
                    yield return entry.Item1.Constant;
                    yield return entry.Item2.Constant;
                }
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue((UInt64)Entries.LongCount());
            foreach(var entry in Entries)
            {
                s.WriteValue(entry.Item1.ConstantID);
                s.WriteValue(entry.Item2.ConstantID);
            }
        }
        public static SuperInterfacesConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            List<(IConstantRef<IInterfaceConstant>, IConstantRef<TypeListConstant>)> supers = new List<(IConstantRef<IInterfaceConstant>, IConstantRef<TypeListConstant>)>();
            ulong count = s.ReadULong();
            for(ulong i=0;i<count;i++)
            {
                ulong ifc = s.ReadULong();
                ulong args = s.ReadULong();
                supers.Add((rcs.ReferenceInterfaceConstant(ifc), rcs.ReferenceTypeListConstant(args)));
            }
            return new SuperInterfacesConstant(id, supers);
        }
    }
}
