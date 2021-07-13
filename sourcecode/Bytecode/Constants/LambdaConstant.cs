using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using Nom.Language;

namespace Nom.Bytecode
{
    public class LambdaConstant : AConstant
    {
        public LambdaConstant(ulong id) : base(ConstantType.CTLambda, id)
        {
        }

        public override IEnumerable<IConstant> Dependencies { get { yield break; } }

        public override void EmitBody(Stream s)
        {

        }
        public static LambdaConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            return new LambdaConstant(id);
        }
    }
}
