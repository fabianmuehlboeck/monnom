#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include "CompileHelpers.h"
#include "PWInt.h"

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
			PWPtr(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			T Load(NomBuilder& builder, llvm::Twine name = "lv", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered)
			{
				return T(MakeLoad(builder, T::GetLLVMType(), wrapped, ordering));
			}
			T InvariantLoad(NomBuilder& builder, llvm::Twine name = "lv", llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered)
			{
				return T(MakeInvariantLoad(builder, T::GetLLVMType(), wrapped, ordering));
			}
			void Store(NomBuilder& builder, T value, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered)
			{
				MakeStore(builder, value, wrapped, ordering);
			}
			void InvariantStore(NomBuilder& builder, T value, llvm::AtomicOrdering ordering = llvm::AtomicOrdering::Unordered)
			{
				MakeInvariantStore(builder, value, wrapped, ordering);
			}
			PWPtr<T> Get(NomBuilder& builder, PWInt32 index, llvm::Twine name = "succ")
			{
				return PWPtr<T>(builder->CreateGEP(T::GetLLVMType(), wrapped, { index }, name));
			}
			PWPtr<T> GetNeg(NomBuilder& builder, PWInt32 index, llvm::Twine name = "prec")
			{
				return PWPtr<T>(builder->CreateGEP(T::GetLLVMType(), wrapped, { builder->CreateNeg(index, "negIndex") }, name));
			}
		};
	}
}