#pragma once

#include <string>
#include "VersionNumber.h"

namespace Nom
{
	namespace Runtime
	{
		class LibraryDependency
		{
		public:
			std::string Name;
			VersionNumber RequiredVersion;
			LibraryDependency(const std::string &name, const VersionNumber &requiredVersion);
		};
	}
}



