#include "Library.h"

namespace Nom
{
	namespace Runtime
	{


		Library::Library(const std::string &name, const std::list<LibraryVersion> &&versions) : Name(name), Versions(versions)
		{
		}


		Library::~Library()
		{
		}

	}
}
