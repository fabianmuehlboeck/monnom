#include "Directory.h"

namespace Nom
{
	namespace Runtime
	{

		Directory::Directory(Path path):dir(FSNamespace::directory_entry(path.path))
		{
		}


		Directory::~Directory()
		{
		}

		std::vector<Directory> Directory::GetDirectories()
		{
			std::vector<Directory> ret;
			for (auto &iter: FSNamespace::directory_iterator(dir.path()))
			{
				ret.push_back(Directory(Path(iter.path().string())));
			}
			return ret;
		}
		std::vector<File> Directory::GetFiles()
		{
			return std::vector<File>();
		}
	}
}
