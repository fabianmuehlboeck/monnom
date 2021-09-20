interface IIterator<T>
{
	T Current();
	IIterator<T> Clone();
	boolean MoveNext();
	boolean MovePrev();
	void SetValue(T x);
}

interface IList<T>
{
	IIterator<T> GetIterator();
	long GetSize();
	void Add(T val);
}
