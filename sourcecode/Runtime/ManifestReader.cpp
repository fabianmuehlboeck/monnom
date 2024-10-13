#include "ManifestReader.h"
#include "pugixml/pugixml.hpp"
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
		Manifest* ManifestReader::ReadManifest(xml_document& doc)
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

			VersionNumber libVersion = VersionNumber(libnode.attribute("major").as_uint(), libnode.attribute("minor").as_uint(), libnode.attribute("revision").as_uint(), libnode.attribute("build").as_uint());
			VersionNumber compFromVersion = libVersion;
			xml_node versionnode = libnode.child("compatiblefrom");
			if (versionnode)
			{
				compFromVersion = VersionNumber(versionnode.attribute("major").as_uint(), versionnode.attribute("minor").as_uint(), versionnode.attribute("revision").as_uint(), versionnode.attribute("build").as_uint());
			}
			VersionNumber compToVersion = libVersion;
			versionnode = libnode.child("compatibleto");
			if (versionnode)
			{
				compToVersion = VersionNumber(versionnode.attribute("major").as_uint(), versionnode.attribute("minor").as_uint(), versionnode.attribute("revision").as_uint(), versionnode.attribute("build").as_uint());
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
					manifest->Dependencies.emplace_back(child.attribute("name").as_string(), VersionNumber(child.attribute("major").as_int(), child.attribute("minor").as_int(), child.attribute("revision").as_int(), child.attribute("buld").as_int()));
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

			xml_node native = libnode.child("native");
			if (native)
			{
				if (!native.empty()) {
					for (auto child : native.children()) {
						if (child.type() == pugi::xml_node_type::node_element && strcmp(child.name(), "library") == 0)
						{
							auto &entry = manifest->NativeLibs.emplace_back(child.attribute("name").as_string());
							for (auto bin : child.children()) {
								if (bin.type() == pugi::xml_node_type::node_element && strcmp(bin.name(), "binary") == 0)
								{
									entry.Binaries.emplace_back(bin.attribute("type").as_string(), bin.attribute("path").as_string(), bin.attribute("platform").as_string(), bin.attribute("os").as_string(), bin.attribute("version").as_string());
								}
							}
						}
					}
				}
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
