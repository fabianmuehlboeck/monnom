#pragma once
#define IMT_ENTRY_SIZE 13
#include <inttypes.h>

namespace Nom
{
	namespace Runtime
	{
		typedef union { void* sarg; void** rest; } RestArgs;
		typedef void* (*IMTLookupMethod)(uint64_t id, uint32_t targcount, uint32_t argcount);
		typedef void* (*IMTMethod)(uint64_t id, IMTLookupMethod lookupMethod, uint32_t targcount, uint32_t argcount, void* arg1, void* arg2, RestArgs argrest);
		enum class VTableKind : unsigned char {
			Class=0
		};
		extern "C" class VTable
		{
		public:
			//void* Methods[0];
			IMTMethod IMTEntries[IMT_ENTRY_SIZE];
			const VTableKind Kind;
			const void* IRLink;

			inline VTable(const IMTMethod *imtEntries, const VTableKind kind, const void* irlink) : Kind(kind), IRLink(irlink)
			{
				for (int i = 0; i < IMT_ENTRY_SIZE; i++)
				{
					IMTEntries[i] = imtEntries[i];
				}
			}
		};
	}
}
