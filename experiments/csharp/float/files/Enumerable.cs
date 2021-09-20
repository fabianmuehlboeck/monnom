using System;
using System.Diagnostics;

using System.Collections.Generic;

class MyArrayList<T> : IEnumerable<T>
{
    private List<T> lst = new List<T>();
    public T get(int index)
    {
        return lst[index];
    }
    public void add(T t)
    {
        lst.Add(t);
    }
    public IEnumerator<T> GetEnumerator()
    {
        return new ArrayListEnumerator(lst);
    }
    private class ArrayListEnumerator : IEnumerator<T>
    {
        private List<T> lst;
        public ArrayListEnumerator(List<T> lst)
        {
            this.lst = lst;
        }
        private int index = -1;
        public bool MoveNext()
        {
            index++;
            return index < lst.Count;
        }
        public T Current()
        {
            return lst[index];
        }
    }
}
class Range : IEnumerable<long>
{
    private long start;
    private long step;
    private long end;
    public Range(long start, long end, long step)
    {
        this.start = start;
        this.end = end;
        this.step = step;
    }
    public Range(long start, long end)
    {
        this.start = start;
        this.end = end;
        this.step = (start <= end ? 1 : -1);
    }
    public IEnumerator<long> GetEnumerator()
    {
        return new RangeEnumerator(start, end, step);
    }
    private class RangeEnumerator : IEnumerator<long>
    {
        private long start;
        private long step;
        private long end;
        public RangeEnumerator(long start, long end, long step)
        {
            this.start = start;
            this.end = end;
            this.step = step;
            this.pos = start - step;
        }
        private long pos;
        public bool MoveNext()
        {
            pos += step;
            return (start <= end ? pos < end : pos > end);
        }
        public long Current()
        {
            return pos;
        }
    }
}
interface IEnumerator<T>
{
    bool MoveNext();
    T Current();
}
interface IEnumerable<T>
{
    IEnumerator<T> GetEnumerator();
}
delegate R Fun2<A, R>(A a);
delegate R Fun3<A, B, R>(A a, B b);
class Enumerable
{
    private class FilterEnumerable<T> : IEnumerable<T>
    {

        private class FilterEnumerator : IEnumerator<T>
        {

            private IEnumerator<T> Base;
            private Fun2<T, bool> Filterfun;
            private T cur;
            public FilterEnumerator(IEnumerator<T> baseEnum, Fun2<T, bool> filter)
            {
                cur = default(T);
                Base = baseEnum;
                Filterfun = filter;
            }
            public T Current()
            {
                return cur;
            }
            public bool MoveNext()
            {
                bool found = false;
                while (!found)
                {
                    if (!Base.MoveNext())
                    {
                        break;
                    }
                    found = Filterfun(Base.Current());

                }
                if (found)
                {
                    cur = Base.Current();
                }
                else
                {
                    cur = default(T);
                }
                return found;
            }
        }
        private IEnumerable<T> Base;
        private Fun2<T, bool> Filter;
        public FilterEnumerable(IEnumerable<T> baseEnum, Fun2<T, bool> filter)
        {
            Base = baseEnum;
            Filter = filter;
        }
        public IEnumerator<T> GetEnumerator()
        {
            return new FilterEnumerator(Base.GetEnumerator(), Filter);
        }
    }
    public static IEnumerable<T> Where<T>(IEnumerable<T> e, Fun2<T, bool> filter)

    {
        return new FilterEnumerable<T>(e, filter);
    }
    private class MapEnumerable<T, S> : IEnumerable<S>
    {

        private class MapEnumerator : IEnumerator<S>
        {

            private IEnumerator<T> Base;
            private Fun2<T, S> Mapper;
            public MapEnumerator(IEnumerator<T> baseEnum, Fun2<T, S> mapper)
            {
                Base = baseEnum;
                Mapper = mapper;
            }
            public bool MoveNext()
            {
                return Base.MoveNext();
            }
            public S Current()
            {
                return Mapper(Base.Current());
            }
        }

        private IEnumerable<T> Base;
        private Fun2<T, S> Mapper;
        public MapEnumerable(IEnumerable<T> baseEnum, Fun2<T, S> mapper)
        {
            Base = baseEnum;
            Mapper = mapper;
        }

        public IEnumerator<S> GetEnumerator()
        {
            return new MapEnumerator(Base.GetEnumerator(), Mapper);
        }
    }
    public static IEnumerable<S> Map<T, S>(IEnumerable<T> e, Fun2<T, S> f)

    {
        return new MapEnumerable<T, S>(e, f);
    }
    public static S Fold<T, S>(IEnumerable<T> e, S baseEnum, Fun3<T, S, S> folder)

    {
        S ret = baseEnum;
        IEnumerator<T> te = e.GetEnumerator();
        while (te.MoveNext())
        {
            T t = te.Current();
            ret = folder(t, ret);
        }
        return ret;
    }
    public static MyArrayList<T> ToList<T>(IEnumerable<T> e)

    {
        MyArrayList<T> ret = new MyArrayList<T>();
        IEnumerator<T> te = e.GetEnumerator();
        while (te.MoveNext())
        {
            T t = te.Current();
            ret.add(t);
        }
        return ret;
    }
    private class FromEnumerable<T> : IEnumerable<T>
    {

        private IEnumerable<T> Base;
        private long StartIndex;
        public FromEnumerable(IEnumerable<T> baseEnum, long startIndex)
        {
            Base = baseEnum;
            StartIndex = startIndex;
        }

        public IEnumerator<T> GetEnumerator()
        {
            IEnumerator<T> e = Base.GetEnumerator();
            long index = 0;
            while (index < StartIndex)
            {
                index = index + 1;
                if (!e.MoveNext())
                {
                    break;
                }
            }
            return e;
        }
    }
    public static IEnumerable<T> From<T>(IEnumerable<T> e, long index)

    {
        return new FromEnumerable<T>(e, index);
    }
}
