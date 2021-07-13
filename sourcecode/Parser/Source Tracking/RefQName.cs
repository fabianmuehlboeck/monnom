using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public class RefQName : AQName<IArgIdentifier<Identifier, IType>>
    {
        public RefQName(bool fromRootOnly, IEnumerable<IArgIdentifier<Identifier, IType>> ids, ISourceSpan locs = null) : base(fromRootOnly, locs, ids.ToArray())
        {
        }
        public RefQName(bool fromRootOnly, ISourceSpan locs = null, params IArgIdentifier<Identifier, IType>[] ids) : base(fromRootOnly, locs, ids)
        {
        }
        //public IEnumerable<IArgIdentifier<String, IType>> IDs { get; }
        //public RefQName()
        //{
        //    IDs = new List<IArgIdentifier<String, IType>>();
        //}
        //public RefQName(IEnumerable<IArgIdentifier<String, IType>> ids)
        //{
        //    this.IDs = ids;
        //}
        //public bool IsEmpty => !IDs.Any(x => true);

        //public bool HasNext => !IsEmpty && IDs.Skip(1).Any(x => true);

        //public IOptional<IArgIdentifier<string, IType>> Current => IDs.FirstOrDefault().InjectOptional();

        //public IOptional<IQName<IArgIdentifier<string, IType>>> Next => (HasNext ? new RefQName(IDs.Skip(1)).InjectOptional<IQName<IArgIdentifier<string, IType>>>() : Optional<IQName<IArgIdentifier<string, IType>>>.Empty);

        //public bool Equals(IQName<IArgIdentifier<string, IType>> other)
        //{
        //    return other.Count() == this.Count() && other.Zip(this.IDs, (x, y) => x.Equals(y)).All(x => x);
        //}

        //public IEnumerator<IArgIdentifier<string, IType>> GetEnumerator()
        //{
        //    return IDs.GetEnumerator();
        //}

        //public override R VisitAstNode<S, R>(IAstNodeVisitor<S, R> visitor, S state)
        //{
        //    return visitor.VisitInvQName(this, state);
        //}

        //IEnumerator IEnumerable.GetEnumerator()
        //{
        //    return GetEnumerator();
        //}

        //public String ElementName
        //{
        //    get
        //    {
        //        if (IsEmpty)
        //        {
        //            return "";
        //        }
        //        return this.Last().Name;
        //    }
        //}

        //public RefQName Concat(RefQName other)
        //{
        //    return new RefQName(IDs.Concat(other.IDs));
        //}
        //public RefQName Snoc(IArgIdentifier<string, IType> elem)
        //{
        //    return new RefQName(IDs.Snoc(elem));
        //}

        //public IQName<X> Transform<X>(Func<IArgIdentifier<string, IType>, X> transformer)
        //{
        //    return AQName<X>.FromIQName(this, transformer);
        //}
    }
    //public partial interface IAstNodeVisitor<in S, out R>
    //{
    //    Func<RefQName, S, R> VisitInvQName { get; }
    //}
}
