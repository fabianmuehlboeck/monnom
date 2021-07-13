#pragma once
#include <string>
#include <list>
#include "LibraryVersion.h"

namespace Nom
{
	namespace Runtime
	{

		class Library
		{
		public:
			const std::string Name;
			const std::list<LibraryVersion> Versions;


			Library(Library&) = delete;
			Library(const Library&) = delete;
			Library(Library&&) = delete;

			Library(const std::string &name, const std::list<LibraryVersion> &&versions);



			~Library();
		};


	}
}
