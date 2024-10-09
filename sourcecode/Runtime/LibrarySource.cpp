#include "LibrarySource.h"
#include "NomModule.h"
#include "StringClass.h"
#include "ObjectClass.h"
#include "NullClass.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "ArrayListClass.h"
#include "MathClass.h"
#include "VoidClass.h"
#include "TimerClass.h"
#include "RangeClass.h"
#include "FunInterface.h"
#include "IPairInterface.h"
#include "IComparableInterface.h"
#include "IEnumerableInterface.h"
#include "IEnumeratorInterface.h"
#include "CharStream.h"
#include "BytecodeReader.h"
#include "pugixml/pugixml.hpp"
#include <fstream>
#include <iostream>
#include "Manifest.h"
#include "ManifestReader.h"

namespace Nom
{
	namespace Runtime
	{
		using namespace pugi;
		namespace LibrarySourceNS
		{
			const llvm::ArrayRef<const NomInterfaceInternal*> GetStdLibInterfaces()
			{
				static const NomInterfaceInternal* ret[] = {
					IPairInterface::GetInstance(),
					IComparableInterface::GetInstance(),
					IEnumerableInterface::GetInstance(),
					IEnumeratorInterface::GetInstance()//,
					//FunInterface::GetFun(0)
				};
				return llvm::ArrayRef<const NomInterfaceInternal*>(ret);
			}
			const llvm::ArrayRef<const NomClassInternal*> GetStdLibClasses()
			{
				static const NomClassInternal* ret[] = {
					NomObjectClass::GetInstance(),
					NomNullClass::GetInstance(),
					NomVoidClass::GetInstance(),
					NomStringClass::GetInstance(),
					NomBoolClass::GetInstance(),
					NomIntClass::GetInstance(),
					NomFloatClass::GetInstance(),
					ArrayListClass::GetInstance(),
					ArrayListEnumeratorClass::GetInstance(),
					NomMathClass::GetInstance(),
					NomTimerClass::GetInstance(),
					RangeClass::GetInstance()
				};
				return llvm::ArrayRef<const NomClassInternal*>(ret);
			}
			//const NomString StringName = "String";
			//const NomString ObjectName = "Object";
			//const NomString NullName = "Null";
		}

		void InternalLibrarySource::GetLibraryVersions(const std::string* name, std::list<LibraryVersion>& versions)
		{
			//std::list<LibraryVersion> versions;
			if (*name == "stdlib")
			{
				versions.push_back(stdlib);
			}
			//return std::move(versions);
		}

		void LocalLibrarySource::GetLibraryVersions(const std::string* name, std::list<LibraryVersion>& versions)
		{
			std::string path = std::string(*Path).append("/").append(*name).append(".manifest");
			const char* fpath = path.c_str();
			std::FILE* file = fopen(fpath, "r");
			if (file != nullptr)
			{
				fclose(file);
				Manifest* manifest = ManifestReader::Instance().ReadManifest(path);
				std::vector<LibraryDependency> dependencies;
				for (auto dep : manifest->Dependencies)
				{
					dependencies.push_back(dep);
				}
				const std::string* name = &(manifest->Name);
				const std::string & mainclassname = manifest->MainClassName();
				VersionNumber libVersion = manifest->Version;
				VersionNumber compFromVersion = manifest->CompatibleFrom;
				VersionNumber compToVersion = manifest->CompatibleTo;
				bool isDeprecated = manifest->IsDeprecated;
				bool isSecurityRisk = manifest->IsSecurityRisk;
				versions.emplace_back(name, mainclassname, libVersion, compFromVersion, compToVersion, isDeprecated, isSecurityRisk, std::move(dependencies), new ManifestComponentSource(Path, manifest, lcs.Program));
			}

		}

		InternalComponentSource::InternalComponentSource(NomProgram* program) : ComponentSource(program)
		{
		}

		const NomClass* InternalComponentSource::LoadClass(NomStringRef name, NomModule* mod)
		{
			for (auto cls : LibrarySourceNS::GetStdLibClasses())
			{
				if (cls->GetName()->compare(*name) == 0)
				{
					mod->AddInternalClass(cls);
					return cls;
				}
			}
			throw name;
		}
		const NomInterface* InternalComponentSource::LoadInterface(NomStringRef name, NomModule* mod)
		{
			for (auto iface : LibrarySourceNS::GetStdLibInterfaces())
			{
				if (iface->GetName()->compare(*name) == 0)
				{
					mod->AddInternalInterface(iface);
					return iface;
				}
			}
			if (name->substr(0, 4) == NomString("Fun_"))
			{
				size_t targcount = std::stol(name->ToStdString().substr(4));
				auto fun = FunInterface::GetFun(targcount-1);
				mod->AddInternalInterface(fun);
				return fun;
			}
			throw name;
		}

		const NomClass* LocalComponentSource::LoadClass(NomStringRef name, NomModule* mod)
		{
			std::string filepath = (*Path) + "/" + (name->ToStdString()) + ".gnil";
			//std::cout << "FILE: " + filepath + "\n";
			std::cout.flush();
			//std::basic_ifstream<unsigned char> fstream(filepath, std::ios::in | std::ios::binary);
			//std::basic_istream<char> stream = std::basic_istream<char>(fstream.rdbuf());
			CharStream cs = CharStream(filepath);
			BytecodeReader br(cs);
			br.readToEnd(mod, this);
			auto lookup = classes.find(name);
			if (lookup == classes.end())
			{
				throw name;
			}
			return lookup->second;
		}

		const NomInterface* LocalComponentSource::LoadInterface(NomStringRef name, NomModule* mod)
		{
			std::string filepath = (*Path) + "/" + (name->ToStdString());
			//std::basic_ifstream<unsigned char> fstream(filepath, std::ios::in | std::ios::binary);
			//std::basic_istream<char> stream = std::basic_istream<char>(fstream.rdbuf());
			CharStream cs = CharStream(filepath);
			BytecodeReader br(cs);
			br.readToEnd(mod, this);
			auto lookup = interfaces.find(name);
			if (lookup == interfaces.end())
			{
				throw name;
			}
			return lookup->second;
		}

		void LocalComponentSource::ReadClass(NomClass* cls)
		{
			classes[cls->GetName()] = cls;
		}
		void LocalComponentSource::ReadInterface(NomInterface* iface)
		{
			interfaces[iface->GetName()] = iface;
		}
		std::tuple<const NativeLib*, const std::string> LocalComponentSource::RequireBinary(std::string name)
		{
			throw new std::exception(("Native code source \"" + name + "\" not found!").c_str());
		}
		const NomClass* ManifestComponentSource::LoadClass(NomStringRef name, NomModule* mod)
		{
			std::string stdname = name->ToStdString();
			for (auto cls : Manifest->Classes)
			{
				if (stdname == cls.Name)
				{
					std::string filepath = (*Path) + "/" + cls.FileName;
					CharStream cs = CharStream(filepath);
					BytecodeReader br(cs);
					br.readToEnd(mod, this);
					auto lookup = classes.find(name);
					if (lookup == classes.end())
					{
						throw name;
					}
					return lookup->second;
				}
			}
			throw name;
		}
		const NomInterface* ManifestComponentSource::LoadInterface(NomStringRef name, NomModule* mod)
		{
			std::string stdname = name->ToStdString();
			for (auto iface : Manifest->Interfaces)
			{
				if (stdname == iface.Name)
				{
					std::string filepath = (*Path) + "/" + iface.FileName;
					CharStream cs = CharStream(filepath);
					BytecodeReader br(cs);
					br.readToEnd(mod, this);
					auto lookup = interfaces.find(name);
					if (lookup == interfaces.end())
					{
						throw name;
					}
					return lookup->second;
				}
			}
			throw name;
		}
		void ManifestComponentSource::ReadClass(NomClass* cls)
		{
			classes[cls->GetName()] = cls;
		}
		void ManifestComponentSource::ReadInterface(NomInterface* iface)
		{
			interfaces[iface->GetName()] = iface;
		}
		std::tuple<const NativeLib *, const std::string> ManifestComponentSource::RequireBinary(std::string name)
		{
			for (auto &nl : this->Manifest->NativeLibs) {
				if (nl.Name == name) {
					return std::make_tuple(&nl, *Path);
				}
			}
			throw new std::exception(("Native code source \"" + name + "\" not found!").c_str());
		}
}
}