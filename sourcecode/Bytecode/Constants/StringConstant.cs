using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    

    public class StringConstant : AConstant, IStringConstant, IEquatable<StringConstant>
    {
        public static StringConstant EmptyStringConstant = new StringConstant("", 0);
        public StringConstant(String value, ulong id) : base(ConstantType.CTString, id)
        {
            this.Value = value;
        }
        public string Value
        {
            get;
            private set;
        }

        private static IConstant[] deps = new IConstant[0];

        public override IEnumerable<IConstant> Dependencies => deps;

        public override void EmitBody(Stream s)
        {
            s.WriteUTF16Str(this.Value);
        }

        public override bool Equals(object obj)
        {
            return Equals(obj as StringConstant);
        }

        public bool Equals(StringConstant other)
        {
            return other != null &&
                   Value == other.Value;
        }

        public override int GetHashCode()
        {
            return -1937169414 + EqualityComparer<string>.Default.GetHashCode(Value);
        }
        public static StringConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            return new StringConstant(s.ReadUTF16Str(), id);
        }
    }
}
