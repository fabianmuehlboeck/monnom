using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class TypeVariableConstant : AConstant, ITypeConstant
    {
        public int Index { get; }
        public TypeVariableConstant(ulong id, int index) : base(ConstantType.CTTypeVar, id)
        {
            Index = index;
        }

        public override IEnumerable<IConstant> Dependencies { get { yield break; } }

        public IType Value => throw new NotImplementedException();

        public override void EmitBody(Stream s)
        {
            s.WriteValue(Index);
        }
        public static TypeVariableConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            int index = s.ReadInt();
            return new TypeVariableConstant(id, index);
        }
    }
}
