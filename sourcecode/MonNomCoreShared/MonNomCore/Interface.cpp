#include "Interface.h"

namespace Nom
{
	namespace Runtime
	{
		MonNomInterface::MonNomInterface(InterfaceFlags flags, int typeArgCount, int superTypesCount, SuperTypeEntry* superTypes) : Flags(flags), TypeArgCount(typeArgCount), SuperTypesCount(superTypesCount), SuperTypes(superTypes)
		{
		}
		MonNomInterface::~MonNomInterface()
		{
		}
	}
}
