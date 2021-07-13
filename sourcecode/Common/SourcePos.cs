using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nom
{
    public class FileSourcePos : ISourcePos
    {
        public FileSourcePos(int line, int col, String file)
        {
            File = file;
            Line = line;
            Column = col;
        }

        public readonly string File;
        public readonly int Line;
        public readonly int Column;

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this.AsSourceSpan());
        }

        public ISourceSpan AsSourceSpan()
        {
            return new FileSourceSpan(this, this);
        }

        public void Visit(ISourcePosVisitor visitor)
        {
            visitor.FileSourcePosAction(this);
        }

        public ISourceSpan SpanTo(ISourcePos end)
        {
            ISourceSpan ret = null;
            end.Visit(new ASourcePosVisitor(
                f => { ret = new FileSourceSpan(this, f); },
                defaultAction: s => { ret = new GenSourceSpan(this, s); }));
                return ret;
        }
    }

    public class LibSourcePos : ISourcePos
    {
        public LibSourcePos(String libname = "<Generated Code>")
        {
            Library = libname;
        }

        public readonly String Library;

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this.AsSourceSpan());
        }

        public ISourceSpan AsSourceSpan()
        {
            return new LibSourceSpan(this, this);
        }

        public void Visit(ISourcePosVisitor visitor)
        {
            visitor.LibSourcePosAction(this);
        }

        public ISourceSpan SpanTo(ISourcePos end)
        {
            ISourceSpan ret = null;
            end.Visit(new ASourcePosVisitor(
                libAction: l => { ret = new LibSourceSpan(this, l); },
                defaultAction: s => { ret = new GenSourceSpan(this, s); }));
            return ret;
        }
        public override string ToString()
        {
            return Library;
        }
    }

    public class GenSourcePos : ISourcePos
    {
        public GenSourcePos()
        {
        }

        public ISourceLocs AsSourceLocs()
        {
            return new SourceLocs(this.AsSourceSpan());
        }

        public ISourceSpan AsSourceSpan()
        {
            return new GenSourceSpan(this, this);
        }

        public void Visit(ISourcePosVisitor visitor)
        {
            visitor.GenSourcePosAction(this);
        }

        public ISourceSpan SpanTo(ISourcePos end)
        {
            return new GenSourceSpan(this, end);
        }
    }

    public class ASourcePosVisitor : ISourcePosVisitor
    {
        public ASourcePosVisitor(Action<FileSourcePos> fileAction = null, Action<LibSourcePos> libAction = null, Action<GenSourcePos> genAction=null, Action<ISourcePos> defaultAction=null)
        {
            FileSourcePosAction = fileAction ?? defaultAction;
            LibSourcePosAction = libAction ?? defaultAction;
            GenSourcePosAction = genAction ?? defaultAction;
        }

        public Action<FileSourcePos> FileSourcePosAction { get; }

        public Action<LibSourcePos> LibSourcePosAction { get; }

        public Action<GenSourcePos> GenSourcePosAction { get; }
    }

}
