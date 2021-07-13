using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace Nom.Parser
{
    public class DeclQName : AQName<DeclIdentifier>
    {
        public DeclQName(IEnumerable<DeclIdentifier> ids, ISourceSpan locs=null):base(false, ids.ToArray(), locs)
        {

        }
        public DeclQName(ISourceSpan locs = null, params DeclIdentifier[] ids) : base(false,locs, ids)
        {
        }

        public DeclQName(ArraySegment<DeclIdentifier> ids, ISourceSpan locs = null) : base(false,ids, locs)
        {
        }

        public DeclQName Concat(DeclQName other)
        {
            return new DeclQName(this.Concat(other), Locs.Start.SpanTo(other.Locs.End));
        }

        public new DeclQName Snoc(DeclIdentifier ident)
        {
            return new DeclQName(this.Snoc(ident), Locs.Start.SpanTo(ident.Locs.End));
        }

        public new IOptional<DeclQName> Next => this.Any(x => true) ? new DeclQName(this.Skip(1), Locs).InjectOptional() : Optional<DeclQName>.Empty;
    }
}
