	function quicksort(l : IList)
	{
		var loIter : IIterator = l.getIterator();
		var hiIter : IIterator = l.getIterator();
		if (loIter.moveNext())
		{
			hiIter.moveNext();
			var lo : number = 0;
			var hi : number = 0;
			while (hiIter.moveNext())
			{
				hi = hi + 1;
			}
			quicksortRec(loIter, hiIter, lo, hi);
		}
	}
	
        function quicksortRec(loIter : IIterator, hiIter : IIterator, lo : number, hi : number)
	{
		if (lo < hi)
		{
			var upper : IIterator = hiIter.clone();
			var lower : IIterator = loIter.clone();
			var losize : number = partition(lower, upper, hi-lo);
			quicksortRec(loIter, upper, lo, lo + losize-1);
			quicksortRec(lower, hiIter, lo + losize, hi);
		}
	}
	
	function partition(loIter : IIterator, hiIter : IIterator, distance : number) : number
	{
		var pivot : number = loIter.current();
		var losize : number = 0;
		while (true)
		{
			while (loIter.current() < pivot)
			{
				loIter.moveNext();
				distance = distance-1;
				losize = losize+1;
			}
			while (hiIter.current() > pivot)
			{
				hiIter.movePrev();
				distance=distance-1;
			}
			if (distance < 0)
			{
				break;
			}
			var buffer : number = loIter.current();
			loIter.setValue(hiIter.current());
			hiIter.setValue(buffer);
			loIter.moveNext();
			losize = losize + 1;
			hiIter.movePrev();
			distance = distance - 2;
		}
		return losize;
	}

}
