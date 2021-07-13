using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface ISourceSpan : IPositionalReference
    {
        ISourcePos Start { get; }
        ISourcePos End { get; }

        ISourceLocs AsSourceLocs();

        void Visit(ISourceSpanVisitor visitor);
    }

    public interface ISourceSpanVisitor
    {
        Action<FileSourceSpan> VisitFileSourceSpan { get; }
        Action<LibSourceSpan> VisitLibSourceSpan { get; }
        Action<GenSourceSpan> VisitGenSourceSpan { get; }
    }
}
