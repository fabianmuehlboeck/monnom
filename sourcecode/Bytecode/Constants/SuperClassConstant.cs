using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Linq;

namespace Nom.Bytecode
{
    public class SuperClassConstant : AConstant
    {
        public IConstantRef<IClassConstant> ClassConstant { get; }
        public IConstantRef<TypeListConstant> ArgumentsConstant { get; }
        public SuperClassConstant(IConstantRef<IClassConstant> clsc, IConstantRef<TypeListConstant> args, ulong id) : base(ConstantType.CTSuperClass, id)
        {
            ClassConstant = clsc;
            ArgumentsConstant = args;
        }

        public override IEnumerable<IConstant> Dependencies => ClassConstant.Singleton().Snoc<IConstantRef<IConstant>>(ArgumentsConstant).Select(c => c.Constant);

        public override void EmitBody(Stream s)
        {
            s.WriteValue(ClassConstant.ConstantID);
            s.WriteValue(ArgumentsConstant.ConstantID);
        }

        public static SuperClassConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong cls = s.ReadULong();
            ulong args = s.ReadULong();
            return new SuperClassConstant(rcs.ReferenceClassConstant(cls), rcs.ReferenceTypeListConstant(args), id);
        }
    }
}
