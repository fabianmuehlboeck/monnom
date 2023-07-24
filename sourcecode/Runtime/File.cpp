#include "File.h"

namespace Nom
{
	namespace Runtime
	{

		File::File(Path path) : dir(path.path)
		{
		}


		File::~File()
		{
		}
	}
}
