    class ListNode
    {
        value : number;
        next : ListNode;
        prev : ListNode;

        constructor(v : number)
        {
            this.value = v;
            this.next = this;
            this.prev = this;
        }
    }

    class List implements IList
    {
	    first : ListNode;
        private size : number;

        constructor(val : number)
        {
            this.size = 1;
            this.first = new ListNode(val);
        }
	
        add(val : number) : void
        {
            var newNode : ListNode = new ListNode(val);
            newNode.prev = this.first.prev;
            newNode.next = this.first;
            this.first.prev = newNode;
            newNode.prev.next = newNode;
            this.size = this.size + 1;
        }
	
        getIterator() : IIterator
        {
            var iter : IIterator = this.makeIterator(this.first);
            return iter;
        }
        
        getSize() : number
        {
            return this.size;
        }

        makeIterator(n : ListNode) : IIterator
        {
            var self : List = this;
            return new IteratorImpl(self, n);
        }
    }

	class IteratorImpl implements IIterator
	{
		private currentNode : ListNode;
		private parent : List;

		constructor(l : List, node : ListNode)
		{
			this.currentNode = node;
			this.parent = l;
		}
	
		movePrev() : boolean
		{
			if (this.currentNode === this.parent.first)
			{
				return false;
			}
			this.currentNode = this.currentNode.prev;
			return true;
		}

		moveNext() : boolean
		{
			if (this.currentNode.next === this.parent.first)
			{
				return false;
			}
			this.currentNode = this.currentNode.next;
			return true;
		}

		current() : number
		{
			return this.currentNode.value;
		}

		setValue(x : number) : void
		{
			this.currentNode.value = x;
		}

		clone() : IIterator
		{
			return this.parent.makeIterator(this.currentNode);
		}
	}

