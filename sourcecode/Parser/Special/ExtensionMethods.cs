using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;

namespace Nom.Parser
{
    public static class ParsingExtensionMethods
    {
        #region EnumerableLink
        private class EnumerableLink<T> : IList<T>
        {
            private IList<T> additions = new List<T>();
            private IList<T> first;
            private IList<T> second;

            public int Count
            {
                get
                {
                    return additions.Count + first.Count + second.Count;
                }
            }

            public bool IsReadOnly
            {
                get
                {
                    return false;
                }
            }

            public T this[int index]
            {
                get
                {
                    if (index < first.Count)
                    {
                        return first[index];
                    }
                    else if (index < first.Count + second.Count)
                    {
                        return second[index - first.Count];
                    }
                    else
                    {
                        return additions[index - first.Count - second.Count];
                    }
                }

                set
                {
                    if (index < first.Count)
                    {
                        first[index] = value;
                    }
                    else if (index < first.Count + second.Count)
                    {
                        second[index - first.Count] = value;
                    }
                    else
                    {
                        additions[index - first.Count - second.Count] = value;
                    }
                }
            }

            public EnumerableLink(IList<T> first, IList<T> second)
            {
                this.first = first;
                this.second = second;
            }

            private class LinkedEnumerator : IEnumerator<T>
            {
                IEnumerator<T> first;
                IEnumerator<T> second;
                IEnumerator<T> additions;
                bool atSecond = false;
                bool atAdditions = false;
                public LinkedEnumerator(EnumerableLink<T> parent)
                {
                    first = parent.first.GetEnumerator();
                    second = parent.second.GetEnumerator();
                    additions = parent.additions.GetEnumerator();
                }
                public T Current
                {
                    get
                    {
                        if (atAdditions)
                        {
                            return additions.Current;
                        }
                        if (atSecond)
                        {
                            return second.Current;
                        }
                        return first.Current;
                    }
                }

                object IEnumerator.Current
                {
                    get
                    {
                        return Current;
                    }
                }

                public void Dispose()
                {
                    first.Dispose();
                    second.Dispose();
                    additions.Dispose();
                }

                public bool MoveNext()
                {
                    if (atAdditions)
                    {
                        return additions.MoveNext();
                    }
                    else
                    {
                        if (atSecond)
                        {
                            if (second.MoveNext())
                            {
                                return true;
                            }
                            else
                            {
                                atAdditions = true;
                                return additions.MoveNext();
                            }
                        }
                        else
                        {
                            if (first.MoveNext())
                            {
                                return true;
                            }
                            else
                            {
                                atSecond = true;
                                return second.MoveNext();
                            }
                        }
                    }
                }

                public void Reset()
                {
                    first.Reset();
                    second.Reset();
                    additions.Reset();
                    atSecond = false;
                    atAdditions = false;
                }
            }
            public IEnumerator<T> GetEnumerator()
            {
                return new LinkedEnumerator(this);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return GetEnumerator();
            }

            public int IndexOf(T item)
            {
                int idx = first.IndexOf(item);
                if (idx < 0)
                {
                    idx = second.IndexOf(item);
                    if (idx < 0)
                    {
                        idx = additions.IndexOf(item);
                    }
                }
                return idx;
            }

            public void Insert(int index, T item)
            {
                throw new NotImplementedException();
            }

            public void RemoveAt(int index)
            {
                if (index < first.Count)
                {
                    first.RemoveAt(index);
                }
                else if (index < first.Count + second.Count)
                {
                    second.RemoveAt(index - first.Count);
                }
                else
                {
                    additions.RemoveAt(index - first.Count - second.Count);
                }
            }

            public void Add(T item)
            {
                additions.Add(item);
            }

            public void Clear()
            {
                first.Clear();
                second.Clear();
                additions.Clear();
            }

            public bool Contains(T item)
            {
                return first.Contains(item) || second.Contains(item) || additions.Contains(item);
            }

            public void CopyTo(T[] array, int arrayIndex)
            {
                if (array.Length < arrayIndex + Count)
                {
                    throw new ArgumentException("Array is too short");
                }
                foreach (T e in this)
                {
                    array[arrayIndex] = e;
                    arrayIndex++;
                }
            }

            public bool Remove(T item)
            {
                if (first.Remove(item))
                {
                    return true;
                }
                else if (second.Remove(item))
                {
                    return true;
                }
                else
                {
                    return additions.Remove(item);
                }
            }
        }

        #endregion

        public static IList<T> Link<T>(this IList<T> e, IList<T> e2)
        {
            return new EnumerableLink<T>(e, e2);
        }

        public static int GetEndLine(this IToken tok)
        {
            return tok.Line + tok.Text.Count(c => c == '\n');
        }

        public static int GetEndColumn(this IToken tok)
        {
            int endLine = tok.Line + tok.Text.Count(c => c == '\n');
            if (endLine == tok.Line)
            {
                return tok.Column + tok.Text.Length;
            }
            else
            {
                return tok.Text.Skip(tok.Text.LastIndexOf('\n') + 1).Count();
            }
        }

        public static string GetFile(this IToken tok)
        {
            return tok.InputStream.SourceName.Replace("\\", "/");
        }

        public static FileSourcePos ToSourcePos(this IToken token)
        {
            return new FileSourcePos(token.Line, token.Column, token.GetFile());
        }

        public static FileSourceSpan ToSourceSpan(this IToken token, IToken end=null)
        {
            return new FileSourceSpan(token.ToSourcePos(), new FileSourcePos((end??token).GetEndLine(), (end??token).GetEndColumn(), (end??token).GetFile()));
        }

        public static ISourceSpan ToSourceSpan(this IEnumerable<IAstNode> nodes)
        {
            return nodes.First().Start.SpanTo(nodes.Last().End);
        }

        public static ISourceSpan ToSourceSpan(this IToken token, ISourcePos pos)
        {
            return token.ToSourcePos().SpanTo(pos);
        }
        
        public static ISourceSpan SpanTo(this ISourcePos pos, IToken tok)
        {
            return pos.SpanTo(tok.ToSourcePos());
        }

        public static ISourceSpan SpanTo(this IToken token, ISourcePos pos)
        {
            return token.ToSourcePos().SpanTo(pos);
        }

        public static Visibility Join(this Visibility visibility, Visibility other)
        {
            if(visibility.EncompassedBy(other))
            {
                return other;
            }
            return visibility;
        }

        public static Visibility Meet(this Visibility visibility, Visibility other)
        {
            if (visibility.EncompassedBy(other))
            {
                return visibility;
            }
            return other;
        }

        public static bool EncompassedBy(this Visibility visibility, Visibility other)
        {
            return visibility < other;
        }
    }
}
