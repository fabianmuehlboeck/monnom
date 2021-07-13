using System;
using System.Collections.Generic;
using System.Text;

namespace Nom
{
    public interface ISourcePos
    {
        ISourceLocs AsSourceLocs();
        ISourceSpan AsSourceSpan();

        ISourceSpan SpanTo(ISourcePos end);

        void Visit(ISourcePosVisitor visitor);
    }
    public interface ISourcePosVisitor
    {
        Action<FileSourcePos> FileSourcePosAction { get; }
        Action<LibSourcePos> LibSourcePosAction { get; }
        Action<GenSourcePos> GenSourcePosAction { get; }
    }
}
