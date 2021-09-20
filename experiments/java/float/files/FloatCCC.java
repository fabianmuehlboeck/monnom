import java.util.function.*;
import java.util.stream.IntStream;
import java.util.ArrayList;

class MyArrayList<T> implements IEnumerable<T>
{
	private ArrayList<T> lst=new ArrayList<T>();
	public T get(int index)
	{
		return lst.get(index);
	}
	public void add(T t)
	{
		lst.add(t);
	}
	public IEnumerator<T> GetEnumerator()
	{
		return new ArrayListEnumerator();
	}
	private class ArrayListEnumerator implements IEnumerator<T>
	{
		private int index=-1;
		public boolean MoveNext()
		{
			index++;
			return index<lst.size();
		}
		public T Current()
		{
			return lst.get(index);
		}
	}
}
class Range implements IEnumerable<Long>
{
	private long start;
	private long step;
	private long end;
	public Range(long start, long end, long step)
	{
		this.start=start;
		this.end=end;
		this.step=step;
	}
	public Range(long start, long end)
	{
		this.start=start;
		this.end=end;
		this.step=(start<=end?1:-1);
	}
	public IEnumerator<Long> GetEnumerator()
	{
		return new RangeEnumerator();
	}
	private class RangeEnumerator implements IEnumerator<Long>
	{
		private long pos=start-step;
		public boolean MoveNext()
		{
			pos+=step;
			return (start<=end?pos<end:pos>end);
		}
		public Long Current()
		{
			return pos;
		}
	}
}
interface IEnumerator<T>
{
	boolean MoveNext();
	T Current();
}
interface IEnumerable<T>
{
	IEnumerator<T> GetEnumerator();
}
interface Fun2<A,R>
{
	R run(A a);
}
interface Fun3<A,B,R>
{
	R run(A a, B b);
}
class Enumerable
{
	private static class FilterEnumerable<T> implements IEnumerable<T>
	{
		private class FilterEnumerator<T> implements IEnumerator<T>
		{
			private IEnumerator<T> Base;
			private Fun2<T,Boolean> Filterfun;
			private T cur;
			public FilterEnumerator(IEnumerator<T> base, Fun2<T,Boolean> filter)
			{
				cur=null;
				Base=base;
				Filterfun=filter;
			}
			public T Current()
			{
				return cur;
			}
			public boolean MoveNext()
			{
				boolean found=false;
				while(!found)
				{
					if(!Base.MoveNext())
					{
						break;
					}
					found=Filterfun.run(Base.Current());
					
				}
				if(found)
				{
					cur=Base.Current();
				}
				else
				{
					cur=null;
				}
				return found;
			}
		}
		private IEnumerable<T> Base;
		private Fun2<T,Boolean> Filter;
		public FilterEnumerable(IEnumerable<T> base, Fun2<T,Boolean> filter)
		{
			Base=base;
			Filter=filter;
		}
		public IEnumerator<T> GetEnumerator()
		{
			return new FilterEnumerator<T>(Base.GetEnumerator(), Filter);
		}
	}
	public static <T> IEnumerable<T> Where(IEnumerable<T> e, Fun2<T,Boolean> filter)
	{
		return new FilterEnumerable<T>(e, filter);
	}
	private static class MapEnumerable<T,S> implements IEnumerable<S>
	{
		private static class MapEnumerator<T,S> implements IEnumerator<S>
		{
			private IEnumerator<T> Base;
			private Fun2<T,S> Mapper;
			public MapEnumerator(IEnumerator<T> base, Fun2<T,S> mapper)
			{
				Base=base;
				Mapper=mapper;
			}
			public boolean MoveNext()
			{
				return Base.MoveNext();
			}
			public S Current()
			{
				return Mapper.run(Base.Current());
			}
		}
	
		private IEnumerable<T> Base;
		private Fun2<T,S> Mapper;
		public MapEnumerable(IEnumerable<T> base, Fun2<T,S> mapper)
		{
			Base=base;
			Mapper=mapper;
		}
		
		public IEnumerator<S> GetEnumerator()
		{
			return new MapEnumerator<T,S>(Base.GetEnumerator(), Mapper);
		}
	}
	public static <T, S> IEnumerable<S> Map(IEnumerable<T> e, Fun2<T,S> f)
	{
		return new MapEnumerable<T,S>(e, f);
	}
	public static <T, S> S Fold(IEnumerable<T> e, S base, Fun3<T, S, S> folder)
	{
		S ret = base;
		IEnumerator<T> te = e.GetEnumerator();
		while(te.MoveNext())
		{
			T t = te.Current();
			ret = folder.run(t, ret);
		}
		return ret;
	}
	public static <T> MyArrayList<T> ToList(IEnumerable<T> e)
	{
		MyArrayList<T> ret = new MyArrayList<T>();
		IEnumerator<T> te = e.GetEnumerator();
		while(te.MoveNext())
		{
			T t = te.Current();
			ret.add(t);
		}
		return ret;
	}
	private static class FromEnumerable<T> implements IEnumerable<T>
	{
		private IEnumerable<T> Base;
		private long StartIndex;
		public FromEnumerable(IEnumerable<T> base, long startIndex)
		{
			Base=base;
			StartIndex=startIndex;
		}
		
		public IEnumerator<T> GetEnumerator()
		{
			IEnumerator<T> e = Base.GetEnumerator();
			long index=0;
			while(index<StartIndex)
			{
				index=index+1;
				if(!e.MoveNext())
				{
					break;
				}
			}
			return e;
		}
	}
	public static <T> IEnumerable<T> From(IEnumerable<T> e, long index)
	{
		return new FromEnumerable<T>(e, index);
	}
}


class Point
{
	private double x;
	private double y;
	private double z;
	public Point(double i)
	{
		x = Math.sin(i);
		y = Math.cos(i) * 3;
		z = (x * x) / 2;
	}
	
	
	public void Normalize()
	{
		double norm = Math.sqrt(x * x + y * y + z * z);
		x = x / norm;
		y = y / norm;
		z = z / norm;
	}
	
	public Point Maximize(Point other)
	{
		if(x < other.x)
		{
			x = other.x;
		}
		if(y < other.y)
		{
			y = other.y;
		}
		if(z < other.z)
		{
			z = other.z;
		}
		return this;
	}
	
	public void Print()
	{
		System.out.println("<"+x+","+y+","+z+">");
	}
}


public class Main
{
	static Point Maximize(MyArrayList<Point> points)
	{
		Point next = points.get(0);
		IEnumerator<Point> pe = Enumerable.From(points,1L).GetEnumerator();
		while(pe.MoveNext())
		{
			Point p = pe.Current();
			next = next.Maximize(p);
		}
		return next;
	}
	static Point Benchmark(long n)
	{
		MyArrayList<Point> points = Enumerable.ToList(Enumerable.Map(new Range(0,n), (i->new Point((double)i))));
		IEnumerator<Point> pe = points.GetEnumerator();
		while(pe.MoveNext())
		{
			Point p = pe.Current();
			p.Normalize();
		}
		return Maximize(points);
	}
	public static void main(String[] args)
	{
		Benchmark(100000);
		Benchmark(100000);
		Benchmark(100000);
		long start=System.nanoTime();
		Point result = Benchmark(100000);
		long end=System.nanoTime();
		System.out.print(((double)(end-start))/1000000000);
		System.out.println(" seconds");
		result.Print();
	}
}
