export module main
{
    class ListNode
    {
        value : any;
        next : any;
        prev : any;

        constructor(v : any)
        {
            this.value = v;
            this.next = this;
            this.prev = this;
        }
    }

    class List
    {
	    first : any;
        private size : any;

        constructor(val : any)
        {
            this.size = 1;
            this.first = new ListNode(val);
        }
	
        add(val : any)
        {
            var newNode : any = new ListNode(val);
            newNode.prev = this.first.prev;
            newNode.next = this.first;
            this.first.prev = newNode;
            newNode.prev.next = newNode;
            this.size = this.size + 1;
        }
	
        getIterator() : any
        {
            var iter : any = this.makeIterator(this.first);
            return iter;
        }
        
        getSize() : any
        {
            return this.size;
        }

        makeIterator(n : any) : any
        {
            var self : any = this;
            return new IteratorImpl(self, n);
        }
    }

	class IteratorImpl
	{
		private currentNode : any;
		private parent : any;

		constructor(l : any, node : any)
		{
			this.currentNode = node;
			this.parent = l;
		}
	
		movePrev() : any
		{
			if (this.currentNode === this.parent.first)
			{
				return false;
			}
			this.currentNode = this.currentNode.prev;
			return true;
		}

		moveNext() : any
		{
			if (this.currentNode.next === this.parent.first)
			{
				return false;
			}
			this.currentNode = this.currentNode.next;
			return true;
		}

		current() : any
		{
			return this.currentNode.value;
		}

		setValue(x : any)
		{
			this.currentNode.value = x;
		}

		clone() : any
		{
			return this.parent.makeIterator(this.currentNode);
		}
	}

    
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
	function quicksort(l : any)
	{
		var loIter : any = l.getIterator();
		var hiIter : any = l.getIterator();
		if (loIter.moveNext())
		{
			hiIter.moveNext();
			var lo : any = 0;
			var hi : any = 0;
			while (hiIter.moveNext())
			{
				hi = hi + 1;
			}
			quicksortRec(loIter, hiIter, lo, hi);
		}
	}
	
        function quicksortRec(loIter : any, hiIter : any, lo : any, hi : any)
	{
		if (lo < hi)
		{
			var upper : any = hiIter.clone();
			var lower : any = loIter.clone();
			var losize : any = partition(lower, upper, hi-lo);
			quicksortRec(loIter, upper, lo, lo + losize-1);
			quicksortRec(lower, hiIter, lo + losize, hi);
		}
	}
	
	function partition(loIter : any, hiIter : any, distance : any) : any
	{
		var pivot : any = loIter.current();
		var losize : any = 0;
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
			var buffer : any = loIter.current();
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
