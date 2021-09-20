using System;
using System.Diagnostics;
public class SortMain
{
	public static void Main(String[] args)
	{
		IList<long> intlist = MakeIntList();
		Test(intlist);
		intlist = MakeIntList();
		Test(intlist);
		intlist = MakeIntList();
		Test(intlist);
		IIterator<long> ilistIter = intlist.GetIterator();
		ilistIter.MoveNext();
		long last = ilistIter.Current();
		while(ilistIter.MoveNext())
		{
			if(ilistIter.Current()<last)
			{
				throw new Exception("sorting failed!");
			}
			last = ilistIter.Current();
		}
	}

	public static void Test(IList<long> list)
	{
		Stopwatch stopwatch = new Stopwatch();
		stopwatch.Start();
		Sort.Quicksort(list);
		stopwatch.Stop();
		Console.WriteLine(((double)(stopwatch.ElapsedMilliseconds))/1000);	
	}
	
	private static long longmod(long a, long b)
	{
		return (a < 0) ? (b-1 - (-a-1) % b) : (a % b);
	}
	
	public static IList<long> MakeIntList()
	{
		IList<long> list = new ListImpl<long>(5L);
		long i =0;
		while(i<100000)
		{
			long opnd1 =  (i * 163841 + 176081);
			long num = longmod(opnd1, 122251);
			list.Add(num);
			i=i+1;
		}
		return list;
	}
}