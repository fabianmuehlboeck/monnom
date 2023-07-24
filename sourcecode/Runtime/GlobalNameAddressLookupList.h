#pragma once
#include <vector>
#include <string>

namespace Nom
{
	namespace Runtime
	{
		void RegisterGlobalForAddressLookup(std::string name);
		std::vector<std::string> &GetGlobalsForAddressLookup();
	}
}
