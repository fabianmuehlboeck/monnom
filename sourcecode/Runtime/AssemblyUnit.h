#pragma once
#include <string>

namespace Nom
{
	namespace Runtime
	{

		class AssemblyUnit
		{
		public:
			const std::string name;
			std::string entryPoint;
			AssemblyUnit(const std::string &name);
			~AssemblyUnit();
		};

	}
}

