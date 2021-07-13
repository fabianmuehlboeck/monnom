using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface IPositionedElement
    {
        ISourcePos Start { get; }
        ISourcePos End { get; }
        ISourceSpan Locs { get; }
    }
}
