    
	var intList :any;
    export function setup()
	{
		intList=makeIntList();
	}
    export function resetup()
	{
		intList=makeIntList();
	}
	export function main()
	{
		test(intList);
//		var ilistIter : any = intList.getIterator();
//		ilistIter.moveNext();
//		var last : any = ilistIter.current();
//		while (ilistIter.moveNext())
//		{
//			if (ilistIter.current() < last)
//			{
//				console.error("sorting failed!");
//			}
//			last = ilistIter.current();
//		}
	}

	function test(list : any)
	{
		quicksort(list);
	}
	
	function makeIntList() : any
	{
		var list : any = new List(5);
		var i : any = 0;
		while (i < 100000)
		{
			var num : any = (i * 163841 + 176081) % 122251;
			list.add(num);
			i = i + 1;
		}
		return list;
	}
