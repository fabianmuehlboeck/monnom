#include "Path.h"
#include <locale>

namespace Nom
{
	namespace Runtime
	{
		using namespace std;
		Path::Path(const std::string &path) : path(FSNamespace::path(path, std::locale("UTF-8")))
		{
		}

		Path::~Path()
		{
		}
	}
}