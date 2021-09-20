

public class Stream
{
	public int First;
	public Func<Stream> Rest;
	
	public Stream(int first, Func<Stream> rest)
	{
		First=first;
		Rest=rest;
	}
	
	public static int Get(Stream s, int n)
	{
		while(n > 0)
		{
			n=(n-1);
			s = s.Rest();
		}
		return s.First;
	}
}