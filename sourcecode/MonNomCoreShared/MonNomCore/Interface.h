#pragma once
#include "VTable.h"


namespace Nom
{
	namespace Runtime
	{
		enum class InterfaceFlags : unsigned char { None = 0x00, IsInterface = 0x01, IsFunctional = 0x02 };
		extern "C" class MonNomInterface;

		extern "C" struct SuperTypeEntry
		{
			const MonNomInterface* const Interface;
			
		};
		extern "C" class MonNomInterface : VTable
		{
		public:
			const InterfaceFlags Flags;
			const unsigned int TypeArgCount;
			const unsigned int SuperTypesCount;
			const SuperTypeEntry* const SuperTypes;

			MonNomInterface(InterfaceFlags flags, int typeArgCount, int superTypesCount, const SuperTypeEntry* superTypes);
			~MonNomInterface();
		};
	}
}