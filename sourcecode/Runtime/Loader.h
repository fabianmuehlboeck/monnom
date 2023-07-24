#pragma once

#include <string>
#include <map>
#include "NomProgram.h"
#include "StringDict.h"

namespace Nom
{
	namespace Runtime
	{
		class LibrarySource;
		class LibraryVersion;
		class Loader
		{
		private:
			Loader();
			mutable NomProgram program;
			mutable StringDict<LibraryVersion *> libraries;
			std::vector<LibrarySource *> sources;
		public:

			Loader(Loader&) = delete;
			Loader(const Loader&) = delete;
			Loader(Loader&&) = delete;
			static const Loader * GetInstance() {
				[[clang::no_destroy]] static const Loader instance;
				return &instance;
			}
			~Loader();

			const LibraryVersion * GetLibrary(const std::string *libraryName) const;

		};

	}
}
