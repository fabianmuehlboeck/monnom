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
			PWBool CompareWith(NomBuilder& builder, PWVTable other) const;
			llvm::Value* ReadHasRawInvoke(NomBuilder& builder) const;
			llvm::Value* ReadMethodPointer(NomBuilder& builder, const PWInt32 index) const;
			PWInt8 ReadKind(NomBuilder& builder) const;
			PWIMTFunction ReadIMTEntry(NomBuilder& builder, const PWInt32 index) const;
		};
	}
}
