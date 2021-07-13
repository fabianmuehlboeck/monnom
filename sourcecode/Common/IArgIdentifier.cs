using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface IArgIdentifier : IArgIdentifier<String, IQName<IArgIdentifier>>, IEquatable<IArgIdentifier>
    {

    }
    public interface IArgIdentifier<NameT, ArgT> : IEquatable<IArgIdentifier<NameT, ArgT>>, INamedReference where ArgT : IReference
    {
        NameT Name
        {
            get;
        }
        IEnumerable<ArgT> Arguments
        {
            get;
        }
        IArgIdentifier<NameT, X> TransformArg<X>(Func<ArgT, X> transformer) where X : IReference;
        IArgIdentifier<NX, AX> Transform<NX, AX>(Func<NameT, NX> nameTransformer, Func<ArgT, AX> argTransformer) where AX : IReference;
    }
}
