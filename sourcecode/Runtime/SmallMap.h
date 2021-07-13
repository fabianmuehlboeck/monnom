#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <utility>

namespace Nom
{
	namespace Runtime
	{
		template<typename K, typename V, class KeyComp = std::less<K>, class KeyEqual = std::equal_to<K>, class Allocator = std::allocator< std::pair<K, V>>>
		class SmallMap
		{
		private:
			std::vector<std::pair<K, V>, Allocator> values;
		public:
			SmallMap()
			{

			}
			~SmallMap()
			{

			}

			void Add(K &key, V &val)
			{
				for (auto iter = values.begin(); iter != values.end(); iter++)
				{
					if (!KeyComp().operator()(*iter, key))
					{
						values.insert(iter, std::make_pair(key, val));
						return;
					}
				}
				values.push_back(std::make_pair(key, val));
			}

			V operator[](K &key)
			{
				for (int i = values.size() - 1; i >= 0; i--)
				{
					if (KeyEqual().operator()(values[i].first, key))
					{
						return values[i].second;
					}
				}
				throw key;
			}
		};

	}
}
