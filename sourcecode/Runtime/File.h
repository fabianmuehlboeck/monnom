#pragma once
#include "Path.h"

namespace Nom
{
	namespace Runtime
	{
		class File
		{
		private:
			FSNamespace::directory_entry dir;
		public:
			File(Path path);
			~File();

			bool Exists()
			{
				return FSNamespace::exists(dir.path()) && FSNamespace::is_directory(dir.path());
			}

			std::string GetFullPath()
			{
				return dir.path().string();
			}

			std::string GetName()
			{
				return dir.path().filename().string();
			}
		};


	}
}
