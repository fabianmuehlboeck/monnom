#pragma once
#include <string>
#include <memory>

namespace Nom
{
	namespace Runtime
	{
		template<typename T>
		using Managed = std::shared_ptr<T>;
		
		template<typename T>
		Managed<T> make_managed(T &t) {
			return std::make_shared<T>(t);
		}
		template<typename T>
		Managed<T> make_managed(T &&t) {
			return std::make_shared<T>(t);
		}

		using ManagedString = Managed<const std::string>;


		/*class Managed
		{
		public:
			Managed();
			~Managed();
		};*/
	}
}
