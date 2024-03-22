#pragma once
#include "PWType.h"

namespace Nom
{
	namespace Runtime
	{
		class PWBotType : public PWType
		{
		public:
			PWBotType(llvm::Value* _wrapped) :PWType(_wrapped)
			{

			}
		};
	}
}