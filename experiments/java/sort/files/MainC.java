public class Main
{
	public static void main(String[] args)
	{
		IList<Long> intlist = MakeIntList();
		Main.Test(intlist);
		intlist = MakeIntList();
		Main.Test(intlist);
		intlist = MakeIntList();
		Main.Test(intlist);
		intlist = MakeIntList();
		Main.Test(intlist);
		IIterator<Long> ilistIter = intlist.GetIterator();
		ilistIter.MoveNext();
		long last = ilistIter.Current();
		while(ilistIter.MoveNext())
		{
			if(ilistIter.Current()<last)
			{
				throw new RuntimeException("sorting failed!");
			}
			last = ilistIter.Current();
		}
	}

	public static void Test(IList<Long> list)
	{
		long start = System.nanoTime();
		Sort.Quicksort(list);
		long end=System.nanoTime();
		System.out.print(((double)(end-start))/1000000000);
		System.out.println(" seconds");
	}
	
	private static long longmod(long a, long b)
	{
		return (a < 0) ? (b-1 - (-a-1) % b) : (a % b);
	}
	
	public static IList<Long> MakeIntList()
	{
		IList<Long> list = new ListImpl<Long>(5L);
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
