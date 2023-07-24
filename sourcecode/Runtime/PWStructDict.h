#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWStructDict : public PWrapper
		{
		public:
			PWStructDict(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{
			}
		};
	}
}
