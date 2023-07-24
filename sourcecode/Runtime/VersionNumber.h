#pragma once
#include <inttypes.h>
#include <stdint.h>

namespace Nom
{
	namespace Runtime
	{
		struct VersionNumber {
			uint16_t Major;
			uint16_t Minor;
			uint16_t Revision;
			uint16_t Build;

			VersionNumber(uint16_t major, uint16_t minor, uint16_t revision, uint16_t build) : Major(major), Minor(minor), Revision(revision), Build(build)
			{

			}

			//VersionNumber(std::string version)
			//{
			//	Major = 1;
			//	Minor = 0;
			//	Revision = 0;
			//	Build = 0;
			//}

			int compare(const VersionNumber &other) const {
				if (this->Major < other.Major) {
					return -1;
				}
				if (this->Major > other.Major) {
					return 1;
				}
				if (this->Minor < other.Minor) {
					return -1;
				}
				if (this->Minor > other.Minor) {
					return 1;
				}
				if (this->Revision < other.Revision) {
					return -1;
				}
				if (this->Revision > other.Revision) {
					return 1;
				}
				if (this->Build < other.Build) {
					return -1;
				}
				if (this->Build > other.Build) {
					return 1;
				}
				return 0;
			}
		};
	}
}
