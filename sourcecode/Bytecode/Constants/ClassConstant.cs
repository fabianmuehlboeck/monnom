using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Nom.Bytecode
{
    public class ClassConstant : AConstant, IClassConstant//, IEquatable<ClassConstant>
    {
        //public static ClassConstant Create(IStringConstant nameConstant)
        //{
        //    return new ClassConstant(nameConstant);
        //}

        //public static ClassConstant Create(string name)
        //{
        //    return new ClassConstant(new StringConstant(name));
        //}

        public ClassConstant(ulong id,IConstantRef<StringConstant> libNameConstant, IConstantRef<StringConstant> nameConstant) : base(ConstantType.CTClass, id)
        {
            this.NameConstant = nameConstant;
            this.LibraryNameConstant = libNameConstant;
        }
        public IConstantRef<StringConstant> LibraryNameConstant
        {
            get;
            set;
        }

        //public ulong TypeArgumentCount
        //{
        //    get;
        //    set;
        //}

        public IOptional<IConstantRef<INamespaceConstant>> ParentConstant
        {
            get;
            set;
        }
        public IConstantRef<StringConstant> NameConstant
        {
            get;
        }

        public string QualifiedName
        {
            get
            {
                return NameConstant.Constant.Value;
                //string ret = NameConstant.Value + "_" + TypeArgumentCount.ToString();
                //if (ParentConstant.HasElem)
                //{
                //    ret = ParentConstant.Elem.QualifiedName + "." + ret;
                //}
                //return ret;
            }
        }

        public override IEnumerable<IConstant> Dependencies
        {
            get
            {
                List<IConstant> ret = new List<IConstant>() { LibraryNameConstant.Constant, NameConstant.Constant };
                if(this.ParentConstant.HasElem)
                {
                    ret.Add(this.ParentConstant.Elem.Constant);
                }
                return ret;
            }
        }

        public override void EmitBody(Stream s)
        {
            //s.WriteByte((byte)BytecodeTopElementType.ClassConstant);
            s.WriteValue(LibraryNameConstant.ConstantID);
            s.WriteValue(NameConstant.ConstantID);
        }
        public static ClassConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong libraryID = s.ReadULong();
            ulong nameID = s.ReadULong();
            return new ClassConstant(id, rcs.ReferenceStringConstant(libraryID), rcs.ReferenceStringConstant(nameID));
        }

        //public override bool Equals(object obj)
        //{
        //    return Equals(obj as ClassConstant);
        //}

        //public bool Equals(ClassConstant other)
        //{
        //    return other != null &&
        //           EqualityComparer<IStringConstant>.Default.Equals(LibraryNameConstant, other.LibraryNameConstant) &&
        //           TypeArgumentCount == other.TypeArgumentCount &&
        //           EqualityComparer<IOptional<INamespaceConstant>>.Default.Equals(ParentConstant, other.ParentConstant) &&
        //           EqualityComparer<IStringConstant>.Default.Equals(NameConstant, other.NameConstant);
        //}

        //public override int GetHashCode()
        //{
        //    var hashCode = -1400951873;
        //    hashCode = hashCode * -1521134295 + EqualityComparer<IStringConstant>.Default.GetHashCode(LibraryNameConstant);
        //    hashCode = hashCode * -1521134295 + TypeArgumentCount.GetHashCode();
        //    hashCode = hashCode * -1521134295 + EqualityComparer<IOptional<INamespaceConstant>>.Default.GetHashCode(ParentConstant);
        //    hashCode = hashCode * -1521134295 + EqualityComparer<IStringConstant>.Default.GetHashCode(NameConstant);
        //    return hashCode;
        //}
    }
}
