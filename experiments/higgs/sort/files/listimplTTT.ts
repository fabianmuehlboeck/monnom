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

