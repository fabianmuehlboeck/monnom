using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.IO;

namespace Nom.Bytecode
{
    public class InterfaceConstant : AConstant, IInterfaceConstant//, IEquatable<InterfaceConstant>
    {
        //public static InterfaceConstant Create(IStringConstant nameConstant)
        //{
        //    return new InterfaceConstant(nameConstant);
        //}

        //public static InterfaceConstant Create(string name)
        //{
        //    return new InterfaceConstant(new StringConstant(name));
        //}

        public InterfaceConstant(ulong id, IConstantRef<StringConstant> libNameConstant, IConstantRef<StringConstant> nameConstant) : base(ConstantType.CTInterface, id)
        {
            this.NameConstant = nameConstant;
            this.LibraryNameConstant = libNameConstant;
        }
        public IConstantRef<StringConstant> LibraryNameConstant
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
                //if (this.ParentConstant.HasElem)
                //{
                //    ret.Add(this.ParentConstant.Elem.Constant);
                //}
                return ret;
            }
        }

        public IEnumerable<MethodDeclRep> MethodDecls
        {
            get;
            set;
        }

        public override void EmitBody(Stream ws)
        {
            ws.WriteValue(LibraryNameConstant.ConstantID);
            ws.WriteValue(NameConstant.ConstantID);
        }

        public static InterfaceConstant Read(Stream s, ulong id, IReadConstantSource rcs)
        {
            ulong libraryID = s.ReadULong();
            ulong nameID = s.ReadULong();
            return new InterfaceConstant(id, rcs.ReferenceStringConstant(libraryID), rcs.ReferenceStringConstant(nameID));
        }
        //public override bool Equals(object obj)
        //{
        //    return Equals(obj as InterfaceConstant);
        //}

        //public bool Equals(InterfaceConstant other)
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
