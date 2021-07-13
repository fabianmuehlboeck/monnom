#include "Loader.h"
#include "NomProgram.h"
#include <list>
#include "LibraryVersion.h"
#include "LibrarySource.h"
#include <cstdlib>
//#include "Runtime.h"
#include "RTConfig.h"
#include <filesystem>

namespace Nom
{
	namespace Runtime
	{



		Loader::Loader()
		{
			//if (NomPath.empty())
			//{
			//	std::string* path = new std::string(std::getenv("NOMBUILDPATH"));
			//	path->append("/NomC/bin/Debug/netcoreapp2.0");
			//	sources.push_back(new LocalLibrarySource(path, &program));
			//}
			//else
			//{
			sources.push_back(new LocalLibrarySource(&NomPath, &program));
			//}
			sources.push_back(new InternalLibrarySource(&program));
			std::string* globalPathStr = new std::string((std::filesystem::weakly_canonical(std::filesystem::path(NomRuntimePath)).parent_path() / "libraries").string());
			sources.push_back(new LocalLibrarySource(globalPathStr, &program));
		}


		Loader::~Loader()
		{
		}

		/*const NomClass * const Loader::GetClass(const std::string *libraryName, const std::string *name) const
		{

		}*/

		const LibraryVersion* Loader::GetLibrary(const std::string* name) const
		{
			auto lib = libraries.find(name);
			if (lib != libraries.end()) {
				return (lib->second);
			}
			std::list<LibraryVersion> versions;
			for (LibrarySource* source : this->sources)
			{
				source->GetLibraryVersions(name, versions);
				if (versions.size() > 0)
				{
					break;
				}
			}
			if (versions.size() == 0)
			{
				throw name;
			}
			//sort versions?
			for (LibraryVersion& version : versions)
			{
				bool ok = true;
				for (LibraryVersion* existing : program.libraries)
				{
					for (const LibraryDependency& dep : existing->Dependencies)
					{
						if (dep.Name == *name)
						{
							if (!version.CompatibleWith(dep))
							{
								ok = false;
								break;
							}
						}
					}
					if (!ok)
					{
						break;
					}
				}
				if (ok)
				{
					LibraryVersion* lib = program.AddLibraryVersion(version);
					for (const LibraryDependency& dep : version.Dependencies)
					{
						GetLibrary(&dep.Name);
					}
					libraries[name] = lib;
					return lib;
				}
			}
			throw name;
		}

		/*bool Loader::LoadAssemblyUnit(const std::string & name, NomProgram * parent) const
		{
			return false;
		}*/

		/*const Library * Loader::GetLibrary(const std::string &name) const {


			libraries.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(name, std::move(versions)));
		}*/

	}
}
