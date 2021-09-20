	function quicksort(l : any)
	{
		var loIter : any = l.getIterator();
		var hiIter : any = l.getIterator();
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
	
        function quicksortRec(loIter : any, hiIter : any, lo : number, hi : number)
	{
		if (lo < hi)
		{
			var upper : any = hiIter.clone();
			var lower : any = loIter.clone();
			var losize : number = partition(lower, upper, hi-lo);
			quicksortRec(loIter, upper, lo, lo + losize-1);
			quicksortRec(lower, hiIter, lo + losize, hi);
		}
	}
	
	function partition(loIter : any, hiIter : any, distance : number) : number
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
