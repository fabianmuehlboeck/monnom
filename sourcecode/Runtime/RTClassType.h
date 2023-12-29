#pragma once
#include "RTType.h"
#include "RTTypePtr.h"
#include "NomClassType.h"
#include "PWClassType.h"

namespace Nom
{
	namespace Runtime
	{
		class RTClassType : public RTPWNTType<PWClassType, NomClassType>
		{

		};

		class RTClassTypePtr : public RTPWTypePtr<RTClassType>
		{

		};
	}
}
