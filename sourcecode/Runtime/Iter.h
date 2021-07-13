#pragma once
namespace Nom
{
	namespace Runtime
	{
		template<typename T>
		class Iter
		{
		private:
			const T * const start;
			const T * current;
			const T * const end;
		public:
			Iter()
			{

			}
			Iter(const Iter &other) : start(other.start), current(other.current), end(other.end)
			{

			}
			~Iter()
			{

			}
			const T * const operator->()
			{
				return current;
			}
			const T & operator*()
			{
				return *current;
			}
			Iter<T> &operator++()
			{
				current++;
				return *this;
			}
		};

	}
}
