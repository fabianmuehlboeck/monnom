#pragma once
#include "../FileSystem/Path.h"
#include "../FileSystem/FileSysDefs.h"
#include <vector>
#include "../FileSystem/File.h"

namespace Nom
{
	namespace Runtime
	{
		class Directory
		{
		private:
			FSNamespace::directory_entry dir;
		public:
			Directory(Path path);
			~Directory();

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

			std::vector<Directory> GetDirectories();
			std::vector<File> GetFiles();
		};
	}
}
