#pragma once

#include <string>
#include "FileSysDefs.h"

namespace Nom
{
	namespace Runtime
	{

		class Path
		{
			friend class Directory;
			friend class File;
		private:
			const FSNamespace::path path;
		public:
			Path(const std::string &path);

			~Path();


		};
	}
}

