using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class MaybeType : AType
    {
        public IType Type
        {
            get;
            private set;
        }

        public override string ReferenceName => Type.ReferenceName + "?";

        public MaybeType(IType type, ISourceSpan locs = null)
            : base(locs ?? new GenSourceSpan())
        {
            this.Type = type;
        }

        public override T Visit<T>(TypeVisitor<T> visitor)
        {
            return visitor.MaybeAction(this);
        }

        //public override bool Equals(IType other)
        //{
        //    TypeVisitor<bool> tv = new TypeVisitor<bool> { MaybeAction = mb => mb.Type.Equals(this.Type) };
        //    return other.Visit(tv);
        //}


        //public override ITDType TransformTD(TDLookup available)
        //{
        //    return TDUnionType.Union(Type.TransformTD(available), BaseType.Null.TransformTD(available));
        //}


        public override R Visit<S, R>(ITypeVisitor<S, R> visitor, S state)
        {
            return visitor.VisitMaybeType(this, state);
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            Type.PrettyPrint(p);
            p.WritePunctuation("?");
        }
    }

    public partial interface ITypeVisitor<in S, out R>
    {
        Func<MaybeType, S, R> VisitMaybeType { get; }
    }
}
