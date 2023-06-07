#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWCastData : public PWrapper
		{
		public:
			PWCastData(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
		};
	}
}