using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface IQName : IQName<IArgIdentifier>, IEquatable<IQName>, INamedReference
    {

    }
    public interface IQName<IdentT> : IEquatable<IQName<IdentT>>, IEnumerable<IdentT>, INamedReference where IdentT : INamedReference
    {
        bool IsFromRootOnly
        {
            get;
        }
        bool IsEmpty
        {
            get;
        }
        bool HasNext
        {
            get;
        }
        IOptional<IdentT> Current
        {
            get;
        }
        IOptional<IQName<IdentT>> Next
        {
            get;
        }
        IQName<X> Transform<X>(Func<IdentT, X> transformer) where X : INamedReference;

        String ElementName { get; }

        IQName<IdentT> Concat(IQName<IdentT> other);
        IQName<IdentT> Snoc(IdentT elem);
    }
}
