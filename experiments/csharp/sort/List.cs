public interface IIterator<T>
{
	T Current();
	IIterator<T> Clone();
	bool MoveNext();
	bool MovePrev();
	void SetValue(T x);
}

public interface IList<T>
{
	IIterator<T> GetIterator();
	long GetSize();
	void Add(T val);
}

