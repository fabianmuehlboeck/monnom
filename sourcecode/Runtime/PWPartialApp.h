#pragma once
#include "PWrapper.h"
#include "PWRefValue.h"

namespace Nom
{
	namespace Runtime
	{
		class PWPartialApp : public PWRefValue
		{
		public:
			PWPartialApp(llvm::Value* _wrapped) : PWRefValue(_wrapped)
			{

			}
		};
	}
}
