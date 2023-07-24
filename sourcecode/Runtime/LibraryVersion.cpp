#include "LibraryVersion.h"
#include "RTModule.h"
#include "Defs.h"
#include "NomProgram.h"

namespace Nom
{
	namespace Runtime
	{
		LibraryVersion::LibraryVersion(const std::string * name, const std::string &mainclassname, const VersionNumber &version, const VersionNumber &compatibleFrom, const VersionNumber &compatibleTo, bool isDeprecated, bool isSecurityRisk, const std::vector<LibraryDependency> &&dependencies, ComponentSource * componentSource) : Name(name), MainClassName(mainclassname), Version(version), CompatibleFrom(compatibleFrom), CompatibleTo(compatibleTo), IsDeprecated(isDeprecated), IsSecurityRisk(isSecurityRisk), Dependencies(dependencies), CSource(componentSource)
		{
		}

		const NomClass * LibraryVersion::GetClass(NomStringRef name) const
		{
			NomModule mod(this->CSource->Program);
			const NomClass * const cls = GetClass(name, &mod);
			mod.EnsureDependenciesResolved();
			RTModule rtm(&mod);
			return cls;
		}

		const NomInterface * LibraryVersion::GetInterface(NomStringRef name) const
		{
			NomModule mod(this->CSource->Program);
			const NomInterface * const iface = GetInterface(name, &mod);
			mod.EnsureDependenciesResolved();
			RTModule rtm(&mod);
			return iface;
		}

	}
}

