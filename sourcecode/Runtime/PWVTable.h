#pragma once
#include "PWrapper.h"
#include "RTVTable.h"
#include "NomBuilder.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWIMTFunction;
		class PWVTable : public PWrapper
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWVTable(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			llvm::Value* ReadHasRawInvoke(NomBuilder& builder) const;
			llvm::Value* ReadMethodPointer(NomBuilder& builder, PWInt32 index) const;
			PWInt8 ReadKind(NomBuilder& builder) const;
			PWIMTFunction ReadIMTEntry(NomBuilder& builder, PWInt32 index) const;
		};
	}
}
