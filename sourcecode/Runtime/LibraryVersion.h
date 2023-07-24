#pragma once

#include <string>
#include <vector>
#include "VersionNumber.h"
#include "LibraryDependency.h"
#include "ComponentSource.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClass;
		class NomInterface;
		class NomProgram;
		class LibraryVersion
		{
		public:
			const std::string * Name;
			const NomString MainClassName;
			VersionNumber Version;
			VersionNumber CompatibleFrom;
			VersionNumber CompatibleTo;
			bool IsDeprecated;
			bool IsSecurityRisk;
			std::vector<LibraryDependency> Dependencies;
			ComponentSource * CSource;

			//LibraryVersion(LibraryVersion&) = delete;
			//LibraryVersion(const LibraryVersion&) = delete;
			//LibraryVersion(LibraryVersion&&) = delete;

			LibraryVersion(const std::string * name, const std::string &mainclassname, const VersionNumber &version, const VersionNumber &compatibleFrom, const VersionNumber &compatibleTo, bool isDeprecated, bool isSecurityRisk, const std::vector<LibraryDependency> &&dependencies, ComponentSource * componentSource);

			bool CompatibleWith(const LibraryDependency &dependency) const {
				return dependency.RequiredVersion.compare(CompatibleFrom) >= 0 && dependency.RequiredVersion.compare(CompatibleTo) <= 0;
			}



			const NomClass * GetClass(NomStringRef name) const;

			const NomInterface * GetInterface(NomStringRef name) const;

			const NomClass * GetClass(NomStringRef name, NomModule *mod) const
			{
				return CSource->GetClass(name, mod);
			}
			const NomInterface * GetInterface(NomStringRef name, NomModule *mod) const
			{
				return CSource->GetInterface(name, mod);
			}

			NomStringRef GetMainClassName() const {
				//static NomString mainclassname = "Main";
				return &MainClassName;
			}
			NomStringRef GetMainMethodName() const {
				[[clang::no_destroy]] static NomString mainmethodname = "Main";
				return &mainmethodname;
			}
		};

	}
}
