#pragma once
#include "VersionNumber.h"
#include <string>
#include <vector>
#include "LibraryDependency.h"

namespace Nom
{
	namespace Runtime
	{

		struct ClassEntry
		{
			const std::string Name;
			const std::string FileName;
			ClassEntry(std::string&& name, std::string&& fileName) : Name(name), FileName(fileName)
			{

			}
		};
		struct InterfaceEntry
		{
			const std::string Name;
			const std::string FileName;
			InterfaceEntry(std::string&& name, std::string&& fileName) : Name(name), FileName(fileName)
			{

			}
		};
		struct NativeEntry
		{
			const std::string Type;
			const std::string Path;
			const std::string Platform;
			const std::string OS;
			const std::string Version;
			NativeEntry(const std::string &type, const std::string& path, const std::string& platform, const std::string& os, const std::string& version) : Type(type), Path(path), Platform(platform), OS(os), Version(version)
			{

			}
		};
		struct NativeLib
		{
			const std::string Name;
			std::vector<NativeEntry> Binaries;
			NativeLib(std::string&& name) : Name(name)
			{

			}
		};

		class Manifest
		{
		private:
			std::string mainClassName;
		public:
			const std::string Name;
			const VersionNumber Version;
			const VersionNumber CompatibleFrom;
			const VersionNumber CompatibleTo;
			const bool IsSecurityRisk;
			const bool IsDeprecated;
			std::vector<LibraryDependency> Dependencies;
			std::vector<ClassEntry> Classes;
			std::vector<InterfaceEntry> Interfaces;
			std::vector<NativeLib> NativeLibs;
			
			Manifest(std::string name, VersionNumber version, VersionNumber compatibleFrom, VersionNumber compatibleTo, bool isSecurityRisk, bool isDeprecated);
			~Manifest();

			void SetMainClass(std::string&& mainClassName)
			{
				this->mainClassName = mainClassName;
			}
			bool HasMainClass() { return mainClassName.length() > 0; }
			const std::string &MainClassName() { return mainClassName; }
		};
	}
}