#pragma once
#include <map>
#include <string>
#include "../Parse/Library.h"
#include "../Parse/LibraryVersion.h"
#include "../Parse/ComponentSource.h"
#include "../Common/StringDict.h"
#include "../Parse/BytecodeTopReadHandler.h"
#include "../Parse/Manifest.h"

namespace Nom
{
	namespace Runtime
	{
		class NomProgram;
		class LibrarySource
		{
		public:
			LibrarySource()
			{

			}
			virtual ~LibrarySource()
			{

			}

			virtual void GetLibraryVersions(const std::string *name, std::list<LibraryVersion> &versions) = 0;
			//std::optional<Library> GetLibrary(const std::string &name)
			//{
			//	//return std::optional();
			//}
		};


		class InternalComponentSource : public ComponentSource
		{
		public:
			InternalComponentSource(NomProgram * program);

			virtual ~InternalComponentSource() override
			{

			}

			const NomClass * LoadClass(NomStringRef name, NomModule * mod) override;
			const NomInterface * LoadInterface(NomStringRef name, NomModule * mod) override;
		};

		class LocalComponentSource : public ComponentSource, public BytecodeTopReadHandler
		{
		/*protected:
			NomProgram * const program;*/
		public:
			const std::string *Path;
			LocalComponentSource(const std::string *path, NomProgram * program) : ComponentSource(program), Path(path)//, program(program)
			{

			}

			virtual ~LocalComponentSource() override
			{

			}

			const NomClass * LoadClass(NomStringRef name, NomModule * mod) override;
			const NomInterface * LoadInterface(NomStringRef name, NomModule * mod) override;
			void ReadClass(NomClass * cls) override;
			void ReadInterface(NomInterface * iface) override;
		};

		class ManifestComponentSource : public ComponentSource, public BytecodeTopReadHandler
		{
		public:
			const std::string* Path;
			const Nom::Runtime::Manifest* const Manifest;
			ManifestComponentSource(const std::string* path, const Nom::Runtime::Manifest * manifest, NomProgram* program) : ComponentSource(program), Path(path), Manifest(manifest)//, program(program)
			{

			}

			virtual ~ManifestComponentSource() override
			{

			}

			const NomClass* LoadClass(NomStringRef name, NomModule* mod) override;
			const NomInterface* LoadInterface(NomStringRef name, NomModule* mod) override;
			void ReadClass(NomClass* cls) override;
			void ReadInterface(NomInterface* iface) override;
		};


		class InternalLibrarySource : public LibrarySource
		{
		private:
			LibraryVersion stdlib;
		public:
			InternalLibrarySource(NomProgram * program) : LibrarySource(), stdlib(new std::string("stdlib"), "", VersionNumber(1, 0, 0, 1), VersionNumber(1, 0, 0, 1), VersionNumber(1, 0, 0, 1), false, false, std::vector<LibraryDependency>(), new InternalComponentSource(program))
			{

			}


			virtual void GetLibraryVersions(const std::string *name, std::list<LibraryVersion> &versions) override;

			virtual ~InternalLibrarySource() override
			{

			}
		};

		class LocalLibrarySource : public LibrarySource
		{
		private:
			LocalComponentSource lcs;
		public:
			const std::string * Path;
			LocalLibrarySource(const std::string * path, NomProgram * program) : LibrarySource(), lcs(path, program), Path(path)
			{

			}

			virtual void GetLibraryVersions(const std::string *name, std::list<LibraryVersion> &versions) override;

			virtual ~LocalLibrarySource() override
			{

			}
		};

	}
}