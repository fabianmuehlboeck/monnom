#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include "PWInt.h"
#include "CompileHelpers.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Twine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/AtomicOrdering.h"
POPDIAGSUPPRESSION

namespace Nom
{
	namespace Runtime
	{
		template <typename T> class PWPtr : public PWrapper
		{
		public:
			static llvm::Type* GetLLVMType()
			{
				return NLLVMPointer(T::GetLLVMType());
			}
			static llvm::Type* GetWrappedLLVMType()
			{
				return POINTERTYPE;
			}
			PWPtr(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			T Load(NomBuilder& builder, llvm::Twine name = "lv", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered) const
			{
				return T(MakeLoad(builder, T::GetWrappedLLVMType(), wrapped, name, ordering));
			}
			T InvariantLoad(NomBuilder& builder, llvm::Twine name = "lv", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered) const
			{
				return T(MakeInvariantLoad(builder, T::GetWrappedLLVMType(), wrapped, name, ordering));
			}
			void Store(NomBuilder& builder, T value, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered) const
			{
				MakeStore(builder, value, wrapped, ordering);
			}
			void InvariantStore(NomBuilder& builder, T value, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered) const
			{
				MakeInvariantStore(builder, value, wrapped, ordering);
			}
			PWPtr<T> Get(NomBuilder& builder, PWInt32 index, llvm::Twine name = "succ") const
			{
				return PWPtr<T>(builder->CreateGEP(T::GetWrappedLLVMType(), wrapped, { index }, name));
			}
			PWPtr<T> GetNeg(NomBuilder& builder, PWInt32 index, llvm::Twine name = "prec") const
			{
				return PWPtr<T>(builder->CreateGEP(T::GetWrappedLLVMType(), wrapped, { builder->CreateNeg(index, "negIndex") }, name));
			}
		};
	}
}
