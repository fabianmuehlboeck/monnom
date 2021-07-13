using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Nom.Language;

namespace Nom.Bytecode
{
    public class FieldRep : IFieldSpec
    {
        public enum FieldRepFlags : byte { None = 0, ReadOnly = 1, Volatile = 2 }
        public FieldRep(IClassSpec container, IConstantRef<IStringConstant> nameConstant, IConstantRef<ITypeConstant> typeConstant, bool isReadonly, bool isVolatile, Visibility visibility)
        {
            Container = container;
            NameConstant = nameConstant;
            TypeConstant = typeConstant;
            Visibility = visibility;
            IsReadonly = isReadonly;
            IsVolatile = isVolatile;
        }
        INamespaceSpec IMember.Container => Container;
        public IClassSpec Container { get; }

        public IConstantRef<IStringConstant> NameConstant { get; }
        public IConstantRef<ITypeConstant> TypeConstant { get; }
        public bool IsReadonly { get; }

        public bool IsVolatile { get; }

        public string Name => NameConstant.Constant.Value;

        public IType Type => TypeConstant.Constant.Value;

        public Visibility Visibility { get; }

        public void WriteByteCode(Stream ws)
        {
            ws.WriteByte((byte)BytecodeInternalElementType.Field);
            ws.WriteValue(NameConstant.ConstantID);
            ws.WriteValue(TypeConstant.ConstantID);
            ws.WriteByte((byte)Visibility);
            FieldRepFlags flags = FieldRepFlags.None;
            if (IsReadonly)
            {
                flags = flags | FieldRepFlags.ReadOnly;
            }
            if (IsVolatile)
            {
                flags = flags | FieldRepFlags.Volatile;
            }
            ws.WriteByte((byte)flags);
        }
        public static FieldRep Read(IClassSpec container, Stream s, IReadConstantSource rcs)
        {
            byte tag = s.ReadActualByte();
            if (tag != (byte)BytecodeInternalElementType.Field)
            {
                throw new NomBytecodeException("Bytecode malformed!");
            }
            var nameconst = rcs.ReferenceStringConstant(s.ReadULong());
            var tconst = rcs.ReferenceTypeConstant(s.ReadULong());
            var visibility = (Visibility)s.ReadActualByte();
            var flags = (FieldRepFlags)s.ReadActualByte();
            return new FieldRep(container, nameconst, tconst, (flags & FieldRepFlags.ReadOnly) == FieldRepFlags.ReadOnly, (flags & FieldRepFlags.Volatile)==FieldRepFlags.Volatile, visibility);
        }
    }
}
