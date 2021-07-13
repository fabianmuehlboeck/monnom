using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface ISourceLocs
    {
        IEnumerable<ISourceSpan> Spans { get; }

        ISourceLocs Append(ISourceLocs other);

        ISourceLocs Append(ISourceSpan other);

        ISourceLocs Append(ISourcePos other);
    }
}
