using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class StructConstant : AConstant
    {
        public StructConstant(ulong id) : base(ConstantType.CTStruct, id)
        {
        }

        public override IEnumerable<IConstant> Dependencies { get { yield break; } }

        public override void EmitBody(Stream s)
        {

        }
        public static StructConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            return new StructConstant(id);
        }
    }
}
