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
		class PWClassType;
		class PWInstanceType;
		class PWMaybeType;
		class PWTypeVar;
		class PWTypeType;
		class PWBotType;
		class PWTopType;
		class PWDynType;
		class PWBool;
		template<typename T> class PWPhi;
		class PWType : public PWrapper
		{
		protected:
			PWPhi<PWBool> GenerateEqPtrCheck(NomBuilder& builder, PWType other, bool checkPtrEq, llvm::BasicBlock** startBlock, llvm::BasicBlock** mergeBlock) const;
		public:
			PWType(llvm::Value* _wrapped) :PWrapper(_wrapped)
			{

			}
			static llvm::Type* GetLLVMType(); 
			static llvm::Type* GetWrappedLLVMType();

			static llvm::MDNode* GetDefaultTypeCasesNode();
			static llvm::MDNode* GetExpectClassTypeNode();
			static llvm::MDNode* GetExpectInstanceTypeNode();
			static llvm::MDNode* GetExpectMaybeTypeNode();
			static llvm::MDNode* GetExpectBotTypeNode();
			static llvm::MDNode* GetExpectTopTypeNode();
			static llvm::MDNode* GetExpectDynTypeNode();
			static llvm::MDNode* GetExpectTypeVarNode();

			void InitializeType(NomBuilder& builder, TypeKind kind, llvm::Value* hash, PWNomType nomtypeptr, PWCastFunction castFun) const;
			llvm::Value* ReadKind(NomBuilder& builder) const;
			llvm::Value* ReadHash(NomBuilder& builder) const;
			PWCastFunction ReadCastFun(NomBuilder& builder) const;

			void GenerateTypeKindSwitch(NomBuilder& builder,
				llvm::MDNode * branchWeights,
				PWSubstStack substitutions,
				std::function<void(NomBuilder&, PWClassType, PWSubstStack)> onClassType,
				std::function<void(NomBuilder&, PWInstanceType, PWSubstStack)> onInstanceType,
				bool resolveTypeVars = true,
				std::function<void(NomBuilder&, PWMaybeType, PWSubstStack)> onMaybeType = nullptr,
				std::function<void(NomBuilder&, PWBotType, PWSubstStack)> onBotType = nullptr,
				std::function<void(NomBuilder&, PWTopType, PWSubstStack)> onTopType = nullptr,
				std::function<void(NomBuilder&, PWDynType, PWSubstStack)> onDynType = nullptr,
				std::function<void(NomBuilder&, PWTypeVar, PWSubstStack)> onTypeVar = nullptr,
				std::function<void(NomBuilder&, PWType, PWSubstStack)> defaultHandler = nullptr) const;

			PWBool IsEq(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWClassType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWInstanceType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWTypeVar other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWMaybeType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWTopType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWBotType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, PWDynType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;
			PWBool IsEq(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false, bool checkPtrEq = true) const;

			PWBool IsSubtype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false) const;
			PWBool IsSubtype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false) const;

			PWBool IsSupertype(NomBuilder& builder, PWType other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false) const;
			PWBool IsSupertype(NomBuilder& builder, NomTypeRef other, PWSubstStack ownSubst, PWSubstStack otherSubst, bool optimistic = false) const;
		};
	}
}
