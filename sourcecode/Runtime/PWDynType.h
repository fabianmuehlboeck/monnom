#pragma once
#include "PWType.h"

namespace Nom
{
	namespace Runtime
	{
		class PWDynType : public PWType
		{
		public:
			PWDynType(llvm::Value* _wrapped) :PWType(_wrapped)
			{

			}
		};
	}
}