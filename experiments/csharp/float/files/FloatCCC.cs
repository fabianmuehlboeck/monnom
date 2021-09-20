
class Point
{
	private double x;
	private double y;
	private double z;
	public Point(double i)
	{
		x = Math.Sin(i);
		y = Math.Cos(i) * 3;
		z = (x * x) / 2;
	}
	
	public void Normalize()
	{
		double norm = Math.Sqrt(x * x + y * y + z * z);
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
		Console.WriteLine("<"+x+","+y+","+z+">");
	}
}


public class Float
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
		MyArrayList<Point> points = Enumerable.ToList(Enumerable.Map<long, Point>(new Range(0,n), (i=>new Point((double)i))));
		IEnumerator<Point> pe = points.GetEnumerator();
		while(pe.MoveNext())
		{
			Point p = pe.Current();
			p.Normalize();
		}
		return Maximize(points);
	}
	public static void Main(String[] args)
	{
		Benchmark(100000);
		Benchmark(100000);
		Stopwatch stopwatch = new Stopwatch();
		stopwatch.Start();
		Point result = Benchmark(100000);
		stopwatch.Stop();
		Console.WriteLine((((double)(stopwatch.ElapsedTicks))/Stopwatch.Frequency).ToString()+" seconds");
		result.Print();
	}
}