#pragma once
#include "PWrapper.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Type.h"
POPDIAGSUPPRESSION
#include "NomBuilder.h"
#include <inttypes.h>
#include "PWTypeArr.h"

namespace Nom
{
	namespace Runtime
	{
		class PWInterface;
		class PWSuperInstance : public PWrapper
		{
		public:
			PWSuperInstance(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWSuperInstance GetEntry(NomBuilder& builder, int32_t index);
			PWSuperInstance GetEntry(NomBuilder& builder, llvm::Value* index);
			PWTypeArr GetTypeArgs(NomBuilder& builder, llvm::AtomicOrdering ordering=llvm::AtomicOrdering::Unordered);
			PWInterface GetInterface(NomBuilder& builder, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered);
		};
	}
}
