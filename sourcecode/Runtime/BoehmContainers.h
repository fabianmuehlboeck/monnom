#pragma once
#include "BoehmAllocator.h"
#include <list>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>

namespace Nom
{
	namespace Runtime
	{
		template <class T>
		class BoehmList : public std::list<T, BoehmAllocator<T>>
		{

		};

		template <class T>
		class BoehmVector : public std::vector<T, BoehmAllocator<T>>
		{
		public:
			BoehmVector()
			{
			}

			BoehmVector(size_t count) : std::vector<T, BoehmAllocator<T>>(count)
			{

			}
		};


		template<class K, class V, typename _cmp = std::less<K>>
		class BoehmMap : public std::map<K, V, _cmp, BoehmAllocator<std::pair<const K, V>>>
		{

		};

		template<class K, class V, typename _hash = std::hash<K>, typename _eq = std::equal_to<K>>
		class BoehmHashMap : public std::unordered_map<K, V, _hash, _eq, BoehmAllocator<std::pair<const K, V>>>
		{

		};
	}
}
