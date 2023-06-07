#pragma once
#include "PWType.h"
#include "NomBuilder.h"
namespace Nom
{
	namespace Runtime
	{
		class PWClassType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			PWClassType(llvm::Value* wrapped) : PWType(wrapped)
			{

			}
		};
	}
}