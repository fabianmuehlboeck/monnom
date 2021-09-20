import java.util.function.*;

public class Main
{

	public static Stream CountFrom(int n)
	{
		return new Stream(n, ()->Main.CountFrom(n+1));
	}
	
	public static Stream Sift(int n, Stream s)
	{
		int f = s.First;
		while(f % n == 0)
		{
			s=s.Rest.get();
			f=s.First;
		}
		final Stream s2 = s;
		return new Stream(f, ()->Main.Sift(n, s2.Rest.get()));
	}
	
	public static Stream Sieve(Stream s)
	{
		int f = s.First;
		return new Stream(f, ()->Main.Sieve(Main.Sift(f, s.Rest.get())));
	}
	
	public static Stream GetPrimes()
	{
		return Main.Sieve(Main.CountFrom(2));
	}

	
	public static void main(String[] args) throws Exception
	{
		Stream.Get(Main.GetPrimes(),9999);
		Stream.Get(Main.GetPrimes(),9999);
		Stream.Get(Main.GetPrimes(),9999);
		long start=System.nanoTime();
		int p = Stream.Get(Main.GetPrimes(),9999);
		long end=System.nanoTime();
		System.out.println(((double)(end-start)/1000000000)+" Seconds");
		System.out.println(p);
	}
}

class Stream
{
	public int First;
	public Supplier<Stream> Rest;
	
	public Stream(int first, Supplier<Stream> rest)
	{
		First=first;
		Rest=rest;
	}
	
	public static int Get(Stream s, int n)
	{
		while(n > 0)
		{
			n=(n-1);
			s = s.Rest.get();
		}
		return s.First;
	}
}