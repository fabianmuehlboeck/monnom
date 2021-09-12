#pragma once

#include <map>
#include "AssemblyUnit.h"
#include <list>
#include "NomClass.h"
#include "NomInterface.h"
#include "LibraryVersion.h"
#include "ComponentSource.h"
#include "VersionNumber.h"
#include "LibraryDependency.h"
#include <vector>

namespace Nom
{
	namespace Runtime
	{
		class Loader;
		class NomModule;
		class NomProgram
		{
		private:
			friend class Loader;
			friend class NomModule;
		public:
			std::list<NomClass *> classes;
			std::list<NomInterface* > interfaces;
			std::list<LibraryVersion *> libraries;

			NomClassLoaded* AddClass(const ConstantID name, const ConstantID typeArgs, const ConstantID superClass, const ConstantID superInterfaces, const NomMemberContext *parent)
			{
				auto elem = new NomClassLoaded(name, typeArgs, superClass, superInterfaces, parent);
				classes.push_back(elem);
				return elem;
			}
			NomInterfaceLoaded* AddInterface(const ConstantID name, const ConstantID typeArgs,  const ConstantID superInterfaces, const NomMemberContext* parent)
			{
				auto elem=new NomInterfaceLoaded(name, typeArgs, superInterfaces, parent);
				interfaces.push_back(elem);
				return elem;
			}
			LibraryVersion* AddLibraryVersion(const std::string *name, const std::string &mainclassname, const VersionNumber &version, const VersionNumber& compatibleFrom, const VersionNumber& compatibleTo, bool isDeprecated, bool isSecurityRisk, const std::vector<LibraryDependency>&& dependencies, ComponentSource* componentSource)
			{
				auto elem = new LibraryVersion(name, mainclassname, version, compatibleFrom, compatibleTo, isDeprecated, isSecurityRisk, std::move(dependencies), componentSource);
				libraries.push_back(elem);
				return elem;
			}
			LibraryVersion* AddLibraryVersion(const LibraryVersion& version)
			{
				auto elem = new LibraryVersion(version);
				libraries.push_back(elem);
				return elem;
			}

			~NomProgram();
			NomProgram();

			NomProgram(NomProgram&) = delete;
			NomProgram(const NomProgram&) = delete;
		};


	}
}
