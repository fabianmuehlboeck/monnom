using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace Nom
{
    public static class ExtensionMethods
    {
        public static bool IsPermutation<T>(this IEnumerable<T> self, IEnumerable<T> other)
        {
            return self.All(x => self.Count(y => y.Equals(x)) == other.Count(y => y.Equals(x))) && other.All(x => self.Contains(x));
        }
        public static bool SetEqual<T>(this IEnumerable<T> self, IEnumerable<T> other)
        {
            return self.All(x => other.Contains(x)) && other.All(x=>self.Contains(x));
        }
        public static IOptional<T> Coalesce<T>(this IOptional<T> self, IOptional<T> other)
        {
            if (self.HasElem)
            {
                return self;
            }
            return other;
        }
        public static IOptional<T> Coalesce<T>(this IOptional<T> self, Func<IOptional<T>> other)
        {
            if (self.HasElem)
            {
                return self;
            }
            return other();
        }
        public static IEnumerable<T> AsEnumerable<T>(this IEnumerator<T> enumerator)
        {
            while (enumerator.MoveNext())
            {
                yield return enumerator.Current;
            }
            yield break;
        }
        public static IOptional<V> GetOptional<K,V>(this IDictionary<K,V> dict, K key)
        {
            if(dict.ContainsKey(key))
            {
                return dict[key].InjectOptional();
            }
            return Optional<V>.Empty;
        }
        public static IDictionary<K, W> MapValues<K, V, W>(this IDictionary<K, V> dict, Func<V, W> transformer)
        {
            return new Dictionary<K, W>(dict.Select(kvp => new KeyValuePair<K, W>(kvp.Key, transformer(kvp.Value))));
        }
        public static IEnumerable<T> Singleton<T>(this T obj)
        {
            yield return obj;
            yield break;
        }

        public static String FillReferences(this String str, params IReference[] references)
        {
            return FillReferences(str, (Exception)null, references);
        }

        public static String FillReferences(this String str, Exception innerException, params IReference[] references)
        {
            StringBuilder sb = new StringBuilder(str.Length);
            StringBuilder isb = new StringBuilder(2);
            for (int i = 0; i < str.Length; i++)
            {
                if (str.Length > i + 1 && str[i] == '\\')
                {
                    i++;
                    sb.Append(str[i]);
                    continue;
                }
                if (str.Length > i + 1 && str[i] == '@' && Char.IsDigit(str[i+1]))
                {
                    isb.Clear();
                    i++;
                    while (i < str.Length && Char.IsDigit(str[i]))
                    {
                        isb.Append(str[i]);
                        i++;
                    }
                    if (isb.Length > 0)
                    {
                        int index = int.Parse(isb.ToString());
                        if (references.Length > index)
                        {
                            sb.Append(references[index].GetReferencePosition());
                        }
                    }
                    if (i < str.Length && str[i] != ';')
                    {
                        i--;
                    }
                    continue;
                }
                if(innerException!=null&&str.Length> i+1 && str[i] == '@' && str[i+1] == 'I')
                {
                    i++;
                    sb.Append(innerException.TargetSite.ToString());
                    continue;
                }
                if (str.Length > i + 1 && str[i] == '$' && Char.IsDigit(str[i + 1]))
                {
                    isb.Clear();
                    i++;
                    while (i < str.Length && Char.IsDigit(str[i]))
                    {
                        isb.Append(str[i]);
                        i++;
                    }
                    if (isb.Length > 0)
                    {
                        int index = int.Parse(isb.ToString());
                        if (references.Length > index)
                        {
                            sb.Append(references[index].GetReferenceName());
                        }
                    }
                    if (i < str.Length && str[i] != ';')
                    {
                        i--;
                    }
                    continue;
                }
                if (innerException != null && str.Length > i + 1 && str[i] == '@' && str[i + 1] == 'M')
                {
                    i++;
                    sb.Append(innerException.Message);
                    continue;
                }
                if (str.Length > i + 1 && str[i] == '%' && Char.IsDigit(str[i + 1]))
                {
                    isb.Clear();
                    i++;
                    while (i < str.Length && Char.IsDigit(str[i]))
                    {
                        isb.Append(str[i]);
                        i++;
                    }
                    if (isb.Length > 0)
                    {
                        int index = int.Parse(isb.ToString());
                        if (references.Length > index)
                        {
                            sb.Append(references[index].GetReferencePosName());
                        }
                    }
                    if (i < str.Length && str[i] != ';')
                    {
                        i--;
                    }
                    continue;
                }
                if (innerException != null && str.Length > i + 1 && str[i] == '%' && str[i + 1] == 'T')
                {
                    i++;
                    sb.Append(innerException.StackTrace);
                    continue;
                }
                sb.Append(str[i]);
            }
            return sb.ToString();
        }

        public static Optional<T> InjectOptional<T>(this T elem)
        {
            if (elem == null)
            {
                return Optional<T>.Empty;
            }
            return Optional<T>.Inject(elem);
        }

        public static void VisitAll<T, V>(this IEnumerable<T> elems, V visitor) where T : IVisitable<V>
        {
            foreach (T elem in elems)
            {
                elem.Visit(visitor);
            }
        }

        public static IEnumerable<T> Flatten<T>(this IEnumerable<IEnumerable<T>> nested)
        {
            foreach (IEnumerable<T> e in nested)
            {
                foreach (T t in e)
                {
                    yield return t;
                }
            }
            yield break;
        }

        public static void MergeDictionaries<K, V>(this IDictionary<K, V> dict, IEnumerable<IEnumerable<KeyValuePair<K, V>>> dicts)
        {
            IEnumerable<K> keys = dicts.Select(d => d.Select(kvp => kvp.Key)).Aggregate((IEnumerable<K>)new List<K>(), (k1, k2) => k1.Concat(k2)).Distinct();
            IEnumerable<K> okeys = dict.Keys.ToList();
            IEnumerable<K> akeys = keys.Where(k => dicts.All(d => d.Select(kvp => kvp.Key).Contains(k))).ToList();
            foreach (K k in okeys.Except(akeys))
            {
                dict.Remove(k);
            }
            foreach (K k in akeys)
            {
                if (dicts.Select(d => d.Single(kvp => kvp.Key.Equals(k)).Value).AllEqual())
                {
                    dict[k] = dicts.First().Single(kvp => kvp.Key.Equals(k)).Value;
                }
                else
                {
                    dict.Remove(k);
                }
            }
        }

        public static bool AllEqual<T>(this IEnumerable<T> items)
        {
            bool ret = true;
            T item = default(T);
            foreach (T t in items)
            {
                if (EqualityComparer<T>.Default.Equals(item, default(T)))
                {
                    item = t;
                    continue;
                }
                if (!t.Equals(item))
                {
                    ret = false;
                    break;
                }
            }
            return ret;
        }

        public static IOptional<T> SingleOrEmpty<T>(this IEnumerable<IOptional<T>> e)
        {
            return e.SingleOrDefault(elem => elem.HasElem) ?? Optional<T>.Empty;
        }

        public static IOptional<T> FirstOrEmpty<T>(this IEnumerable<IOptional<T>> e)
        {
            return e.FirstOrDefault(elem => elem.HasElem) ?? Optional<T>.Empty;
        }

        public static IEnumerable<T> Cons<T>(this IEnumerable<T> e, T elem)
        {
            yield return elem;
            foreach (T t in e)
            {
                yield return t;
            }
            yield break;
        }

        public static IEnumerable<T> Snoc<T>(this IEnumerable<T> e, T elem)
        {
            foreach (T t in e)
            {
                yield return t;
            }
            yield return elem;
            yield break;
        }

        public static IEnumerable<IEnumerable<T>> GetRows<T>(this T[,] arr)
        {
            for (int i = 0; i < arr.GetLength(0); i++)
            {
                yield return arr.GetRow(i);
            }
            yield break;
        }

        public static IEnumerable<T> GetRow<T>(this T[,] arr, int rownum)
        {
            for (int i = 0; i < arr.GetLength(1); i++)
            {
                yield return arr[rownum, i];
            }
            yield break;
        }

        public static IEnumerable<String> Indent(this IEnumerable<string> lines, int baseIndent = 0, int indentDepth = 4)
        {
            int curIndent = baseIndent;
            foreach (string line in lines)
            {
                switch (line.Trim())
                {
                    case "{":
                        yield return new String(Enumerable.Repeat(' ', curIndent).Concat("{").ToArray());
                        curIndent += indentDepth;
                        break;
                    case "}":
                        curIndent = Math.Max(curIndent - indentDepth, 0);
                        yield return new String(Enumerable.Repeat(' ', curIndent).Concat("}").ToArray());
                        break;
                    default:
                        yield return new String(Enumerable.Repeat(' ', curIndent).Concat(line.Trim()).ToArray());
                        break;
                }
            }
            yield break;
        }

        public static IEnumerable<uint> RangeTo(this uint to, uint start = 0)
        {
            while (start <= to)
            {
                yield return start++;
            }
            yield break;
        }

        public static String AppendSeparator(this String str, String separator)
        {
            String trm = str.Trim();
            if (trm.Length > 0)
            {
                return trm + separator;
            }
            return str;
        }

        public static String PrependSeparator(this String str, String separator)
        {
            String trm = str.Trim();
            if (trm.Length > 0)
            {
                return separator + trm;
            }
            return str;
        }
    }
}
