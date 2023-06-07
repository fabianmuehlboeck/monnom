#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWCastFunction : public PWrapper
		{
		public:
			PWCastFunction(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
		};
	}
}