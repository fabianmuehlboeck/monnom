#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWDispatchPair : public PWrapper
		{
		public:
			PWDispatchPair(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
		};
	}
}
