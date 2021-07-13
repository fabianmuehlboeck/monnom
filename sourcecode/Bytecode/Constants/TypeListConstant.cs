using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public class TypeListConstant : AConstant
    {
        public TypeListConstant(ulong id, IEnumerable<IConstantRef<ITypeConstant>> types) : base(ConstantType.CTTypeList, id)
        {
            this.TypeConstants = types.ToList();
        }

        public override IEnumerable<IConstant> Dependencies => TypeConstants.Select(tc=>tc.Constant);

        public IEnumerable<IConstantRef<ITypeConstant>> TypeConstants
        {
            get;
        }

        public override void EmitBody(Stream ws)
        {
            ws.WriteValue((uint)TypeConstants.Count());
            foreach(IConstantRef<ITypeConstant> tcr in TypeConstants)
            {
                ws.WriteValue(tcr.ConstantID);
            }
        }
        public static TypeListConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            List<IConstantRef<ITypeConstant>> entries = new List<IConstantRef<ITypeConstant>>();
            uint count = s.ReadUInt();
            for (uint i = 0; i < count; i++)
            {
                ulong tp = s.ReadULong();
                entries.Add(rcs.ReferenceTypeConstant(tp));
            }
            return new TypeListConstant(id, entries);
        }
    }
}
