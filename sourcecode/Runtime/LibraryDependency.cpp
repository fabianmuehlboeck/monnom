#include "LibraryDependency.h"

namespace Nom
{
	namespace Runtime
	{
		LibraryDependency::LibraryDependency(const std::string &name, const VersionNumber &requiredVersion) : Name(name), RequiredVersion(requiredVersion)
		{
		}
	}
}

