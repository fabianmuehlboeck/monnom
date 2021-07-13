using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class BottomTypeConstant : AConstant, ITypeConstant
    {
        public BottomTypeConstant(ulong id) : base(ConstantType.CTBottom, id)
        {
        }

        public override IEnumerable<IConstant> Dependencies { get { yield break; } }

        public IType Value => BotType.Instance;

        public override void EmitBody(Stream s)
        {
        }

        public static BottomTypeConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            return new BottomTypeConstant(id);
        }
    }
}
