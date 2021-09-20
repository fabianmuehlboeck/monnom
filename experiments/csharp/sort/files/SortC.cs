
public class Sort
{
	public static void Quicksort(IList<long> l)
	{
		IIterator<long> loIter = l.GetIterator();
		IIterator<long> hiIter = l.GetIterator();
		if(loIter.MoveNext())
		{
			hiIter.MoveNext();
			long lo=0;
			long hi=0;
			while(hiIter.MoveNext())
			{
				hi=hi+1;
			}
			QuicksortRec(loIter, hiIter, lo, hi);
		}
	}
	
    private static void QuicksortRec(IIterator<long> loIter, IIterator<long> hiIter, long lo, long hi)
	{
		if(lo < hi)
		{
			IIterator<long> upper = hiIter.Clone();
			IIterator<long> lower= loIter.Clone();
			long losize = Partition(lower, upper, hi-lo);
			QuicksortRec(loIter, upper, lo, lo + losize-1);
			QuicksortRec(lower, hiIter, lo + losize, hi);
		}
	}
	
	private static long Partition(IIterator<long> loIter, IIterator<long> hiIter, long distance)
	{
		long pivot = loIter.Current();
		long losize=0;
		while(true)
		{
			while(loIter.Current().CompareTo(pivot)<0)
			{
				loIter.MoveNext();
				distance=distance-1;
				losize=losize+1;
			}
			while(hiIter.Current().CompareTo(pivot)>0)
			{
				hiIter.MovePrev();
				distance=distance-1;
			}
			if(distance<0)
			{
				break;
			}
			long buffer=loIter.Current();
			loIter.SetValue(hiIter.Current());
			hiIter.SetValue(buffer);
			loIter.MoveNext();
			losize=losize+1;
			hiIter.MovePrev();
			distance=distance-2;
		}
		return losize;
	}
}
