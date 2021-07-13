using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    class DynamicTypeConstant : AConstant, ITypeConstant
    {
        public DynamicTypeConstant(ulong id) : base(ConstantType.CTDynamicType, id)
        {
        }

        private static IType dyntype = new Language.DynamicType();
        public IType Value => dyntype;

        public override IEnumerable<IConstant> Dependencies { get { yield break; } }

        public override void EmitBody(Stream s)
        {
        }

        public static DynamicTypeConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            return new DynamicTypeConstant(id);
        }
    }
}
