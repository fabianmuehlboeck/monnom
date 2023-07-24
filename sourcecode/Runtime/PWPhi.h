#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T> class PWPhi : public T
		{
		public:
			static PWPhi Create(NomBuilder& builder, unsigned int incoming, llvm::Twine name="")
			{
				return builder->CreatePHI(T::GetWrappedLLVMType(), incoming, name);
			}
			static PWPhi CreatePtr(NomBuilder& builder, unsigned int incoming, llvm::Twine name = "")
			{
				return builder->CreatePHI(NLLVMPointer(T::GetWrappedLLVMType()), incoming, name);
			}
			PWPhi(llvm::PHINode * _wrapped) : T(_wrapped)
			{

			}
			llvm::PHINode* operator->() const
			{
				return static_cast<llvm::PHINode*>(this->wrapped);
			}
			operator llvm::PHINode* () const
			{
				return static_cast<llvm::PHINode*>(this->wrapped);
			}
			operator T () const
			{
				return T(this->wrapped);
			}
		};
	}
}
