using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class IntersectionTypeConstant : AConstant, IIntersectionConstant
    {
        public IntersectionTypeConstant(ulong id, IConstantRef<TypeListConstant> componentsConstant) : base(ConstantType.CTIntersection, id)
        {
            ComponentsConstant = componentsConstant;
        }

        public override IEnumerable<IConstant> Dependencies => throw new NotImplementedException();

        public IConstantRef<TypeListConstant> ComponentsConstant { get; }

        public IType Value => throw new NotImplementedException();

        public override void EmitBody(Stream s)
        {
            s.WriteValue(ComponentsConstant.ConstantID);
        }

        public static IntersectionTypeConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong components = s.ReadULong();
            return new IntersectionTypeConstant(id, rcs.ReferenceTypeListConstant(components));
        }
    }
}
