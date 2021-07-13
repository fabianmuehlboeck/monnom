using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Nom.Bytecode
{
    public class NamespaceConstant : AConstant, INamespaceConstant//, IEquatable<NamespaceConstant>
    {
        //public static NamespaceConstant Create(IStringConstant nameConstant)
        //{
        //    return new NamespaceConstant(nameConstant);
        //}

        //public static NamespaceConstant Create(string name)
        //{
        //    return new NamespaceConstant(new StringConstant(name));
        //}

        protected NamespaceConstant(IConstantRef<StringConstant> nameConstant, ulong id) : base(ConstantType.CTInterface, id)
        {
            this.NameConstant = nameConstant;
        }

        public IOptional<IConstantRef<INamespaceConstant>> ParentConstant
        {
            get;
            set;
        }
        public IConstantRef<StringConstant> NameConstant
        {
            get;
            set;
        }

        public string QualifiedName
        {
            get
            {
                return NameConstant.Constant.Value;
                //    string ret = NameConstant.Value;
                //    if (ParentConstant.HasElem)
                //    {
                //        ret = ParentConstant.Elem.QualifiedName + "." + ret;
                //    }
                //    return ret;
            }
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                List<IConstant> ret = new List<IConstant>() { NameConstant.Constant };
                if (this.ParentConstant.HasElem)
                {
                    ret.Add(this.ParentConstant.Elem.Constant);
                }
                return ret;
            }
        }

        public override void EmitBody(Stream s)
        {
            throw new NotImplementedException();
        }
        public static NamespaceConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            throw new NotImplementedException();
        }

        //public override bool Equals(object obj)
        //{
        //    return Equals(obj as NamespaceConstant);
        //}

        //public bool Equals(NamespaceConstant other)
        //{
        //    return other != null &&
        //           EqualityComparer<IOptional<INamespaceConstant>>.Default.Equals(ParentConstant, other.ParentConstant) &&
        //           EqualityComparer<IStringConstant>.Default.Equals(NameConstant, other.NameConstant);
        //}

        //public override int GetHashCode()
        //{
        //    var hashCode = -1543007117;
        //    hashCode = hashCode * -1521134295 + EqualityComparer<IOptional<INamespaceConstant>>.Default.GetHashCode(ParentConstant);
        //    hashCode = hashCode * -1521134295 + EqualityComparer<IStringConstant>.Default.GetHashCode(NameConstant);
        //    return hashCode;
        //}
    }
}
