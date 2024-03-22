#pragma once
#include "PWType.h"

namespace Nom
{
	namespace Runtime
	{

		class PWTopType : public PWType
		{
		public:
			PWTopType(llvm::Value* _wrapped) :PWType(_wrapped)
			{

			}
		};
	}
}