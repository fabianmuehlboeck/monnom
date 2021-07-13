using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom.Parser
{
    public class ArrayType : ATypeLiteral
    {
        public IType Type
        {
            get;
            private set;
        }

        public override string ReferenceName => Type.ReferenceName + "[]";

        public ArrayType(IType type, ISourceSpan locs) : base(locs)
        {
            this.Type = type;
        }

        public override T Visit<T>(TypeVisitor<T> visitor)
        {
            return visitor.ArrayAction(this);
        }

        public override R Visit<S, R>(ITypeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitArrayType(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Type.PrettyPrint(p);
            p.WritePunctuation("[]");
            p.WriteWhitespace();
        }
    }

    public partial interface ITypeVisitor<in S, out R>
    {
        Func<ArrayType, S, R> VisitArrayType { get; }
    }
}
