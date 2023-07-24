#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWVMPtr : public PWrapper
		{
		public:
			PWVMPtr(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
		};
	}
}
