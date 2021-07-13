using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class AQName<IdentT> : AAstNode, IQName<IdentT> where IdentT : INamedReference
    {
        private ArraySegment<IdentT> ids;
        public AQName(bool fromRootOnly, ISourceSpan locs = null, params IdentT[] ids) : base(locs)
        {
            this.ids = ids;
            this.IsFromRootOnly = fromRootOnly;
        }
        public AQName(bool fromRootOnly, ArraySegment<IdentT> ids, ISourceSpan locs = null) : base(locs)
        {
            this.ids = ids;
            this.IsFromRootOnly = fromRootOnly;
        }

        public bool IsEmpty => ids.Count == 0;

        public bool HasNext => ids.Count > 1;

        public IOptional<IdentT> Current => ids.FirstOrDefault().InjectOptional();

        public IOptional<IQName<IdentT>> Next => ids.Any(x => true) ? new AQName<IdentT>(false, ids.Slice(1), Locs).InjectOptional() : Optional<AQName<IdentT>>.Empty;

        public virtual string ElementName
        {
            get
            {
                if(IsEmpty)
                {
                    return "";
                }
                return ids.Last().ToString();
            }
        }

        public bool IsFromRootOnly { get; }

        public string ReferencePosName => ReferenceName + " (" + ReferencePosition + ")";

        public string ReferenceName => String.Join(".", ids.Select(id=>id.ReferenceName));

        public bool Equals(IQName<IdentT> other)
        {
            return this.ids.Count == other.Count() & this.ids.Zip(other, (x, y) => x.Equals(y)).All(x => x);
        }

        public IEnumerator<IdentT> GetEnumerator()
        {
            return ids.GetEnumerator();
        }

        public IQName<X> Transform<X>(Func<IdentT, X> transformer) where X: INamedReference
        {
            return new AQName<X>(IsFromRootOnly, ids.Select(transformer).ToArray(), Locs);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        {
            return visitor.GetAQNameVisitor<IdentT>()(this, state);
        }

        public IQName<IdentT> Concat(IQName<IdentT> other)
        {
            return new AQName<IdentT>(IsFromRootOnly, Locs, ids.Concat(other).ToArray());
        }

        public IQName<IdentT> Snoc(IdentT elem)
        {
            return new AQName<IdentT>(IsFromRootOnly, Locs, ids.Snoc(elem).ToArray());
        }

        public override void PrettyPrint(PrettyPrinter p)
        {
            p.PrintList(ids, ".");
        }
    }

    public partial interface IAstNodeVisitor<in S, out R>
    {
        Func<AQName<IdentT>, S, R> GetAQNameVisitor<IdentT>() where IdentT : INamedReference;
    }
}
