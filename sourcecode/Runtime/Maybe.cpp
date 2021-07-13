#include "Maybe.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T>
		Maybe<T>::Maybe() : hasValue(false)
		{
		}

		template<typename T>
		Maybe<T>::Maybe(T &&t) : hasValue(true)
		{
			char * ptr = this->data;
			T * tptr = reinterpret_cast<T*>(ptr);
			T &tt = t;
			*tptr = tt;
		}

		template<typename T>
		Maybe<T>::~Maybe()
		{
		}

	}
}