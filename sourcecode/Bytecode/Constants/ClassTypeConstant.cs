using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Nom.Language;

namespace Nom.Bytecode
{
    public class ClassTypeConstant : AConstant, ITypeConstant
    {
        public ClassTypeConstant(ulong id, IConstantRef<INamedConstant> namedType, IConstantRef<TypeListConstant> args):base(ConstantType.CTClassType, id)
        {
            NamedTypeConstant = namedType;
            TypeArgumentsConstant = args;
        }
        public IConstantRef<INamedConstant> NamedTypeConstant { get; }

        public IType Value => throw new NotImplementedException();

        public IConstantRef<TypeListConstant> TypeArgumentsConstant
        {
            get;
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                yield return NamedTypeConstant.Constant;
                yield return TypeArgumentsConstant.Constant;
                yield break;
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue(NamedTypeConstant.ConstantID);
            s.WriteValue(TypeArgumentsConstant.ConstantID);
        }
        public static ClassTypeConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong namedType = s.ReadULong();
            ulong args = s.ReadULong();
            return new ClassTypeConstant(id, rcs.ReferenceNamedConstant(namedType), rcs.ReferenceTypeListConstant(args));
        }
    }
}
