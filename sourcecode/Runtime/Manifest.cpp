#include "Manifest.h"

namespace Nom
{
	namespace Runtime
	{

		Manifest::Manifest(std::string name, VersionNumber version, VersionNumber compatibleFrom, VersionNumber compatibleTo, bool isSecurityRisk, bool isDeprecated) : Name(name), Version(version), CompatibleFrom(compatibleFrom), CompatibleTo(compatibleTo), IsSecurityRisk(isSecurityRisk), IsDeprecated(isDeprecated)
		{
		}

		Manifest::~Manifest()
		{
		}
	}
}
