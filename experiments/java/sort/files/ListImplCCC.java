class ListImpl<T> implements IList<T>
{
	private ListNode first;	
	private long Size;
	public ListImpl(T val)
	{
		Size=1;
		first = new ListNode(val);
	}
	
	public void Add(T val)
	{
		ListNode newNode = new ListNode(val);
		newNode.prev = first.prev;
		newNode.next = first;
		first.prev = newNode;
		newNode.prev.next = newNode;
		Size=Size+1;
	}
	
	public IIterator<T> GetIterator()
	{
		IIterator<T> iter = this.MakeIterator(first);
		return iter;
	}
	public long GetSize()
	{
		return Size;
	}

	private class ListNode
	{
		public ListNode(T v)
		{
			value=v;
			next=this;
			prev=this;
		}
		public T value;
		public ListNode next;
		public ListNode prev;
	}

	public IIterator<T> MakeIterator(ListNode n)
	{
		ListImpl<T> self = this;
		return new IteratorImpl(self, n);
	}

	private class IteratorImpl implements IIterator<T>
	{
		ListNode currentNode;
		ListImpl<T> parent;
		public IteratorImpl(ListImpl<T> l, ListNode node)
		{
			currentNode = node;
			parent = l;
		}
	
		public boolean MovePrev()
		{
			if(currentNode==parent.first)
			{
				return false;
			}
			currentNode = currentNode.prev;
			return true;
		}
		public boolean MoveNext()
		{
			if(currentNode.next==parent.first)
			{
				return false;
			}
			currentNode=currentNode.next;
			return true;
		}
		public T Current()
		{
			return currentNode.value;
		}
		public void SetValue(T x)
		{
			currentNode.value = x;
		}
		public IIterator<T> Clone()
		{
			return parent.MakeIterator(currentNode);
		}
	}
}
