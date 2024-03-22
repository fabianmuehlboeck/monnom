#pragma once
#include "PWTypeArr.h"
#include "NomBuilder.h"
#include "PWType.h"

namespace Nom
{
	namespace Runtime
	{
		class PWSubstStack;
		class PWInterface;
		class PWClassType : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWClassType(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			PWInterface ReadClassDescriptorLink(NomBuilder& builder);
			PWTypeArr TypeArgumentsPointer(NomBuilder& builder);			
			
			PWBool IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true);

			PWBool IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);

			PWBool IsSupertype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
		};
	}
}
