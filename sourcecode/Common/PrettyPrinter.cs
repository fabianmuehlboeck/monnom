using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

namespace Nom
{
    public interface IPrettyPrintable
    {
        void PrettyPrint(PrettyPrinter p);
    }
    public class PrettyPrinter : IDisposable
    {
        private StreamWriter writer;
        private int currentLine = -1;
        bool whitespaceWaiting = false;
        bool atNewLine = true;
        private int indent=0;
        public PrettyPrinter(StreamWriter writer)
        {
            this.writer=writer;
        }
        public PrettyPrinter(FileInfo file)
        {
            this.writer = new StreamWriter(file.OpenWrite());
        }

        public PrettyPrinter(Stream stream)
        {
            this.writer = new StreamWriter(stream);
        }

        public void Write(string str, ISourcePos start)
        {
            EnsureIndent();
            int line = GetLine(start);
            StringBuilder sb = new StringBuilder();
            if (line>currentLine)
            {
                writer.WriteLine();
                int column = GetColumn(start);
                sb.Append(' ', Math.Max(column,indent * 4));
                currentLine = line;
            }
            else
            {
                if(whitespaceWaiting)
                {
                    sb.Append(" ");
                }
            }
            sb.Append(str);
            writer.Write(sb.ToString());
            whitespaceWaiting = false;
        }
        public void IncreaseIndent()
        {
            indent++;
        }
        public void DecreaseIndent()
        {
            indent--;
        }
        public void WriteWhitespace()
        {
            if (!atNewLine)
            {
                whitespaceWaiting = true;
            }
        }
        public void WritePunctuation(string punctuation)
        {
            EnsureIndent();
            writer.Write(punctuation);
            whitespaceWaiting = false;
        }
        public void WriteKeyword(string keyword)
        {
            EnsureIndent();
            if (whitespaceWaiting)
            {
                writer.Write(" ");
            }
            writer.Write(keyword);
            whitespaceWaiting = true;
        }
        public void WriteLine()
        {
            if(!atNewLine)
            {
                writer.WriteLine();
            }
            atNewLine = true;
        }
        private void EnsureIndent()
        {
            if(atNewLine)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(' ', indent * 4);
                writer.Write(sb.ToString());
                currentLine++;
                whitespaceWaiting = false;
            }
            atNewLine = false;
        }

        public void PrintList<T>(IEnumerable<T> nodes, string punctuation = null, bool whitespace = true) where T : IPrettyPrintable
        {
            bool first = true;
            foreach (var node in nodes)
            {
                if (punctuation != null && !first)
                {
                    WritePunctuation(punctuation);
                    if (whitespace) { WriteWhitespace(); }
                }
                node.PrettyPrint(this);
                first = false;
            }
        }

        public void PrintMembers<T>(IEnumerable<T> nodes, string punctuation=null, bool whitespace = true) where T: IPositionedElement, IPrettyPrintable
        {
            bool first = true;
            foreach (var node in nodes.OrderBy(node => node.Start, PosComparer.Instance))
            {
                if(punctuation!=null && !first)
                {
                    WritePunctuation(punctuation);
                    if (whitespace) { WriteWhitespace(); }
                }
                node.PrettyPrint(this);
                first = false;
            }
        }

        private static int GetLine(ISourcePos pos)
        {
            int line = -1;
            pos.Visit(new ASourcePosVisitor(fsp => { line = fsp.Line; }, defaultAction: sp => { }));
            return line;
        }

        private static int GetColumn(ISourcePos pos)
        {
            int column = 0;
            pos.Visit(new ASourcePosVisitor(fsp => { column = fsp.Column; }, defaultAction: sp => { }));
            return column;
        }

        public void Dispose()
        {
            writer.Dispose();
        }

        private class PosComparer : IComparer<ISourcePos>
        {
            public static PosComparer Instance => new PosComparer();
            private PosComparer() { }

            public int Compare(ISourcePos x, ISourcePos y)
            {
                int xline = GetLine(x);
                int yline = GetLine(y);
                if(xline == yline)
                {
                    int xcol = GetColumn(x);
                    int ycol = GetColumn(y);
                    if(xcol == ycol)
                    {
                        return 0;
                    }
                    else if(xcol < ycol)
                    {
                        return -1;
                    }
                    else
                    {
                        return 1;
                    }
                }
                else if(xline < yline)
                {
                    return -1;
                }
                else
                {
                    return 1;
                }
            }
        }
    }
}
