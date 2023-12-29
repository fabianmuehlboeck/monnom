#pragma once
#include "PWrapper.h"
#include "Defs.h"
#include "NomBuilder.h"
#include "PWInt.h"
#include "NomTypeDecls.h"

namespace Nom
{
	namespace Runtime
	{
		class PWNomType;
		class PWCastFunction;
		class PWSubstStack;
		class PWType : public PWrapper
		{
		public:
			PWType(llvm::Value* _wrapped) :PWrapper(_wrapped)
			{

			}
			static llvm::Type* GetLLVMType(); 
			static llvm::Type* GetWrappedLLVMType();
			void InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun);
			llvm::Value* ReadKind(NomBuilder& builder);
			llvm::Value* ReadHash(NomBuilder& builder);
			PWCastFunction ReadCastFun(NomBuilder& builder);

			PWBool IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
			PWBool IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);

			PWBool IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
			PWBool IsSubtype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);

			PWBool IsSupertype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
			PWBool IsSupertype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false);
		};
	}
}
