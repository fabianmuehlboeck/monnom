#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWFloat : public PWrapper
		{
		public:
			PWFloat(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
		};
	}
}
