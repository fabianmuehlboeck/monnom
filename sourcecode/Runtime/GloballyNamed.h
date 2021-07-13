#pragma once
#include "llvm/ADT/Twine.h"
namespace Nom
{
	namespace Runtime
	{
		class GloballyNamed
		{
		private:
			const std::string *globalNamePrefix;
			mutable std::string globalName = "";
		public:
			GloballyNamed(const std::string *prefix) : globalNamePrefix(prefix) {}
			~GloballyNamed() {}
			std::string &GetGlobalName() const
			{
				static size_t counter = 0;
				if (globalName.length() == 0)
				{
					counter++;
					std::string str = std::to_string(counter);
					globalName = *globalNamePrefix + str;
				}
				return globalName;
			}
		};
	}
}

