#include "ManifestReader.h"
#include "VersionNumber.h"

using namespace pugi;
namespace Nom
{
	namespace Runtime
	{
		ManifestReader::ManifestReader()
		{
		}

		ManifestReader::~ManifestReader()
		{
		}

		


		Manifest* ManifestReader::ReadManifest(void* buffer, size_t bufsize)
		{
			xml_document doc;
			xml_parse_result result = doc.load_buffer_inplace_own(buffer, bufsize);
			return ReadManifest(doc);
		}
		Manifest* ManifestReader::ReadManifest(xml_document &doc)
		{
			xml_node libnode = doc.child("nomlibrary");
			auto formatVersion = libnode.attribute("fversion");
			if (!(formatVersion.empty() || formatVersion.as_int() == 2))
			{
				throw new std::exception(); //newer format version than can be understood
			}
			auto libname = libnode.attribute("name");
			auto deps = libnode.child("dependencies");
			auto classes = libnode.child("classes");
			auto interfaces = libnode.child("interfaces");
			auto mainclass = libnode.child("mainclass");
			
			VersionNumber libVersion = VersionNumber(static_cast<uint16_t>(libnode.attribute("major").as_uint()), static_cast<uint16_t>(libnode.attribute("minor").as_uint()), static_cast<uint16_t>(libnode.attribute("revision").as_uint()), static_cast<uint16_t>(libnode.attribute("build").as_uint()));
			VersionNumber compFromVersion = libVersion;
			xml_node versionnode = libnode.child("compatiblefrom");
			if (versionnode)
			{
				compFromVersion = VersionNumber(static_cast<uint16_t>(versionnode.attribute("major").as_uint()), static_cast<uint16_t>(versionnode.attribute("minor").as_uint()), static_cast<uint16_t>(versionnode.attribute("revision").as_uint()), static_cast<uint16_t>(versionnode.attribute("build").as_uint()));
			}
			VersionNumber compToVersion = libVersion;
			versionnode = libnode.child("compatibleto");
			if (versionnode)
			{
				compToVersion = VersionNumber(static_cast<uint16_t>(versionnode.attribute("major").as_uint()), static_cast<uint16_t>(versionnode.attribute("minor").as_uint()), static_cast<uint16_t>(versionnode.attribute("revision").as_uint()), static_cast<uint16_t>(versionnode.attribute("build").as_uint()));
			}
			bool isDeprecated = false;

			if (libnode.attribute("deprecated"))
			{
				isDeprecated = true;
			}
			bool isSecurityRisk = false;
			if (libnode.attribute("securityrisk"))
			{
				isSecurityRisk = true;
			}

			Manifest* manifest = new Manifest(libname.as_string(), libVersion, compFromVersion, compToVersion, isSecurityRisk, isDeprecated);
			if (!deps.empty())
			{
				for (auto child : deps.children())
				{
					manifest->Dependencies.emplace_back(child.attribute("name").as_string(), VersionNumber(static_cast<uint16_t>(child.attribute("major").as_uint()), static_cast<uint16_t>(child.attribute("minor").as_uint()), static_cast<uint16_t>(child.attribute("revision").as_uint()), static_cast<uint16_t>(child.attribute("build").as_uint())));
				}
			}
			if (!classes.empty())
			{
				for (auto child : classes.children())
				{
					manifest->Classes.emplace_back(child.attribute("qname").as_string(), child.attribute("file").as_string());
				}
			}
			if (!interfaces.empty())
			{
				for (auto child : interfaces.children())
				{
					manifest->Interfaces.emplace_back(child.attribute("qname").as_string(), child.attribute("file").as_string());
				}
			}
			if (!mainclass.empty())
			{
				manifest->SetMainClass(mainclass.attribute("name").as_string());
			}
			return manifest;
		}

		Manifest* ManifestReader::ReadManifest(std::string& path)
		{
			xml_document doc;
			xml_parse_result result = doc.load_file(path.c_str());
			return ReadManifest(doc);
		}

	}
}
