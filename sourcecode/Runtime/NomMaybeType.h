#pragma once
#include "NomType.h"
#include "GloballyNamed.h"
#include "RTTypeHead.h"

namespace Nom
{
	namespace Runtime
	{
		class NomMaybeType : public NomType, public GloballyNamed
		{
		private:
			NomMaybeType(NomTypeRef potentialType);
			mutable RTTypeHead head;
		public:
			const NomTypeRef PotentialType;
			static NomMaybeTypeRef GetMaybeType(NomTypeRef potentialType);
			virtual ~NomMaybeType() override;
			// Inherited via NomType
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
			virtual bool ContainsVariables() const override;
			virtual llvm::Value* GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const override;
			virtual size_t GetHashCode() const override;
			virtual bool IsSubtype(NomTypeRef other, bool optimistic = false) const override;
			virtual bool IsSubtype(NomBottomTypeRef other, bool optimistic = false) const override;
			virtual bool IsSubtype(NomDynamicTypeRef other, bool optimistic = false) const override;
			virtual bool IsSubtype(NomClassTypeRef other, bool optimistic = false) const override;
			virtual bool IsSubtype(NomTopTypeRef other, bool optimistic = false) const override;
			virtual bool IsSubtype(NomTypeVarRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomDynamicTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomBottomTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomClassTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomTopTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomTypeVarRef other, bool optimistic = false) const override;
			virtual bool IsDisjoint(NomTypeRef other) const override;
			virtual bool IsDisjoint(NomBottomTypeRef other) const override;
			virtual bool IsDisjoint(NomClassTypeRef other) const override;
			virtual bool IsDisjoint(NomTopTypeRef other) const override;
			virtual bool IsDisjoint(NomTypeVarRef other) const override;
			virtual bool PossiblyPrimitive() const override;
			virtual bool UncertainlyPrimitive() const override;
			virtual NomTypeRef SubstituteSubtyping(const NomSubstitutionContext* context) const override;
			virtual llvm::Type* GetLLVMType() const override;
			virtual const std::string GetSymbolRep() const override;
			virtual TypeKind GetKind() const override;
			virtual intptr_t GetRTElement() const override;
			virtual NomClassTypeRef GetClassInstantiation(const NomNamed* named) const override;

			// Inherited via NomType
			virtual bool IsSubtype(NomMaybeTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomMaybeTypeRef other, bool optimistic = false) const override;
			virtual bool IsDisjoint(NomMaybeTypeRef other) const override;

			typedef void* (*InitFunctionPointer)(void*, void*, size_t, const NomType*);
			static InitFunctionPointer GetCPPInitializerFunction();
			static llvm::Function* GetInitializerFunction(llvm::Module& mod);

			// Inherited via NomType
			virtual TypeReferenceType GetTypeReferenceType() const override;

			// Inherited via NomType
			virtual bool ContainsVariableIndex(int index) const override;
		};
	}
}