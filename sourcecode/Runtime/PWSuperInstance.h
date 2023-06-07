#pragma once
#include "PWrapper.h"
#include "llvm/IR/Type.h"
#include "NomBuilder.h"
#include <inttypes.h>

namespace Nom
{
	namespace Runtime
	{
		class PWSuperInstance : public PWrapper
		{
		public:
			PWSuperInstance(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			static llvm::Type* GetLLVMType();
			PWSuperInstance GetEntry(NomBuilder& builder, int32_t index);
			PWSuperInstance GetEntry(NomBuilder& builder, llvm::Value* index);
		};
	}
}