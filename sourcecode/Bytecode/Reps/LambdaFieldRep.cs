using System;
using System.Collections.Generic;
using System.Text;
using Nom.Language;
using System.Linq;
using System.IO;

namespace Nom.Bytecode
{
    public class LambdaFieldRep
    {
        public IConstantRef<StringConstant> FieldNameConstant { get; }
        public IConstantRef<ITypeConstant> TypeConstant { get; }
        public LambdaFieldRep(IConstantRef<StringConstant> fieldName, IConstantRef<ITypeConstant> type)
        {
            FieldNameConstant = fieldName;
            TypeConstant = type;
        }

        public void WriteByteCode(Stream ws)
        {
            ws.WriteValue(FieldNameConstant.ConstantID);
            ws.WriteValue(TypeConstant.ConstantID);
        }

        public static LambdaFieldRep Read(Stream s, IReadConstantSource rcs)
        {
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var typeconst = rcs.ReferenceTypeConstant(s.ReadULong());
            return new LambdaFieldRep(nameconst, typeconst);
        }
    }
}
