#pragma once
#include "CharStream.h"
PUSHDIAGSUPPRESSION
#include "pugixml/pugixml.hpp"
POPDIAGSUPPRESSION
#include "Manifest.h"
#include <iostream>

namespace Nom
{
	namespace Runtime
	{

		class ManifestReader
		{
		private:
			ManifestReader();
		public:
			static ManifestReader& Instance() { [[clang::no_destroy]] static ManifestReader instance; return instance; }
			~ManifestReader();

			Manifest* ReadManifest(pugi::xml_document &doc);
			Manifest* ReadManifest(std::string& path);
			Manifest* ReadManifest(void* buffer, size_t bufsize);
		};

	}
}
