    var intList : IList;
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
	}

	function test(list : IList)
	{
		quicksort(list);
	}
	
	function makeIntList() : IList
	{
		var list : IList = new List(5);
		var i : number = 0;
		while (i < 100000)
		{
			var num : number = (i * 163841 + 176081) % 122251;
			list.add(num);
			i = i + 1;
		}
		return list;
	}
