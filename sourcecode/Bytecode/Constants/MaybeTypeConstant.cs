using Nom.Language;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class MaybeTypeConstant : AConstant, ITypeConstant
    {
        public readonly IConstantRef<ITypeConstant> PotentialType;
        public MaybeTypeConstant(ulong id, IConstantRef<ITypeConstant> type) : base(ConstantType.CTMaybeType, id)
        {
            PotentialType = type;
        }
        private IType value = null;
        public IType Value
        {
            get
            {
                if(value==null)
                {
                    value = new MaybeType(PotentialType.Constant.Value);
                }
                return value;
            }
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                yield return PotentialType.Constant;
            }
        }

        public override void EmitBody(Stream s)
        {
            s.WriteValue(PotentialType.ConstantID);
        }
        public static MaybeTypeConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong potentialType = s.ReadULong();
            return new MaybeTypeConstant(id, rcs.ReferenceTypeConstant(potentialType));
        }
    }
}
