using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class SourceLocs : ISourceLocs
    {
        public SourceLocs(IEnumerable<ISourceLocs> locs)
        {
            Spans = locs.Select(l => l.Spans).Flatten().ToList();
        }

        public SourceLocs(params ISourceSpan[] spans)
        {
            Spans = spans;
        }

        public SourceLocs(IEnumerable<ISourceSpan> spans)
        {
            Spans = spans;
        }
        
        public IEnumerable<ISourceSpan> Spans { get; }

        public ISourceLocs Append(ISourceLocs other)
        {
            return new SourceLocs(Spans.Concat(other.Spans));
        }

        public ISourceLocs Append(ISourceSpan other)
        {
            return new SourceLocs(Spans.Snoc(other));
        }

        public ISourceLocs Append(ISourcePos other)
        {
            return Append(other.AsSourceSpan());
        }
    }
}
