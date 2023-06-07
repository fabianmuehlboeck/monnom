#pragma once
#include "PWrapper.h"

namespace Nom
{
	namespace Runtime
	{
		class PWSignature : public PWrapper
		{
		public:
			PWSignature(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
		};
	}
}