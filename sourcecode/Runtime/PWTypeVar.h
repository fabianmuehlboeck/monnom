#pragma once
#include "PWType.h"
#include "NomBuilder.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWTypeVar : public PWType
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWTypeVar(llvm::Value* _wrapped) : PWType(_wrapped)
			{

			}
			PWInt32 ReadIndex(NomBuilder& builder);

			PWBool IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst);
			PWBool IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst);

			PWBool IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
			PWBool IsSubtype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);

			PWBool IsSupertype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
			PWBool IsSupertype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
		};
	}
}
