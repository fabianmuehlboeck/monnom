#pragma once
#include "PWrapper.h"
#include "NomBuilder.h"
#include "PWInt.h"
#include "PWTypeArr.h"

namespace Nom
{
	namespace Runtime
	{
		class PWType;
		class PWTypeVar;
		class PWSubstStack : public PWrapper
		{
		public:
			llvm::Value* invariantID = nullptr;
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			static PWSubstStack Alloca(NomBuilder& builder, PWSubstStack substStack, PWTypeArr args, llvm::Twine name = "substStack");
			PWSubstStack(llvm::Value* _wrapped, llvm::Value* _invariantID=nullptr);
			PWType Pop(NomBuilder& builder, PWInt32 varIndex, PWSubstStack* newStackAddr) const;
			PWType Pop(NomBuilder& builder, PWTypeVar var, PWSubstStack* newStackAddr) const;
			PWSubstStack ReadSuccStack(NomBuilder& builder, llvm::Twine name="nextStack") const;
			PWTypeArr ReadSubstitutions(NomBuilder& builder, llvm::Twine name="substs") const;

			void Release(NomBuilder& builder) const;
		};
	}
}
