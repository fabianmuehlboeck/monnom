#include "Path.h"
#include <locale>

namespace Nom
{
	namespace Runtime
	{
		using namespace std;
		Path::Path(const std::string &_path) : path(FSNamespace::path(_path, std::locale("UTF-8")))
		{
		}

		Path::~Path()
		{
		}
	}
}
