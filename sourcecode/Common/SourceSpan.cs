using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class FileSourceSpan : ISourceSpan
    {
        public FileSourceSpan(FileSourcePos start, FileSourcePos end) : base()
        {
            StartPos = start;
            EndPos = end;
            if (start.File != end.File)
            {
                EndPos = StartPos;
                //throw new InternalException("Invalid Source Span - different files!");
            }
            if (start.Line > end.Line || (start.Line == end.Line && start.Column > end.Column))
            {
                EndPos = StartPos;
                //throw new InternalException("Invalid source span - start behind end!");
            }
        }

        public readonly FileSourcePos StartPos;
        public readonly FileSourcePos EndPos;

        public ISourcePos Start => StartPos;

        public ISourcePos End => EndPos;

        public string ReferencePosition => StartPos.File + ":" + StartPos.Line + ":" + StartPos.Column;

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this);
        }

        public void Visit(ISourceSpanVisitor visitor)
        {
            visitor.VisitFileSourceSpan(this);
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }

    public class LibSourceSpan : ISourceSpan
    {
        public LibSourceSpan(LibSourcePos start, LibSourcePos end)
        {
            StartPos = start;
            EndPos = end;
            if (start.Library != end.Library)
            {
                throw new Exception("Invalid source span - different libraries!");
            }
        }

        public readonly LibSourcePos StartPos;
        public readonly LibSourcePos EndPos;

        public ISourcePos Start => StartPos;

        public ISourcePos End => EndPos;

        public string ReferencePosition => StartPos.ToString();

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this);
        }

        public void Visit(ISourceSpanVisitor visitor)
        {
            visitor.VisitLibSourceSpan(this);
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }
    }

    public class GenSourceSpan : ISourceSpan
    {
        public GenSourceSpan(ISourcePos start = null, ISourcePos end = null)
        {
            Start = start??new GenSourcePos();
            End = end??new GenSourcePos();
        }

        public ISourcePos Start { get; }

        public ISourcePos End { get; }

        public string ReferencePosition => "<Generated Code>";

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this);
        }

        public void PrettyPrint(PrettyPrinter p)
        {
            throw new NotImplementedException();
        }

        public void Visit(ISourceSpanVisitor visitor)
        {
            visitor.VisitGenSourceSpan(this);
        }
    }
}
