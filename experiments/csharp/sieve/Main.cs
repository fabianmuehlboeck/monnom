using System;
using System.Diagnostics;

public class SieveMain
{

	public static Stream CountFrom(int n)
	{
		return new Stream(n, ()=>CountFrom(n+1));
	}
	
	public static Stream Sift(int n, Stream s)
	{
		int f = s.First;
		while(f % n == 0)
		{
			s=s.Rest();
			f=s.First;
		}
		return new Stream(f, ()=>Sift(n, s.Rest()));
	}
	
	public static Stream Sieve(Stream s)
	{
		int f = s.First;
		return new Stream(f, ()=>Sieve(Sift(f, s.Rest())));
	}
	
	public static Stream GetPrimes()
	{
		return Sieve(CountFrom(2));
	}

	
	public static void Main(String[] args)
	{
		Stream.Get(GetPrimes(),9999);
		Stream.Get(GetPrimes(),9999);
		Stopwatch stopwatch=new Stopwatch();
		stopwatch.Start();
		int p = Stream.Get(GetPrimes(),9999);
		stopwatch.Stop();
		Console.WriteLine((stopwatch.ElapsedMilliseconds/1000.0)+" Seconds");
		Console.WriteLine(p);
	}
}