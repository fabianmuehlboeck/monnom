#pragma once
#include "NomType.h"
#include "Defs.h"
#include "CompileEnv.h"
#include "llvm/IR/IRBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomBottomType : public NomType
		{
		private:
			NomBottomType();
		public:
			virtual ~NomBottomType() override;
			static NomBottomTypeRef Instance() { static NomBottomType type; return &type; }
			
			bool IsSubtype(NomTypeRef other, bool optimistic=false) const override;
			bool IsSubtype(NomBottomTypeRef other, bool optimistic=false) const override;
			bool IsSubtype(NomClassTypeRef other, bool optimistic=false) const override;
			bool IsSubtype(NomTopTypeRef other, bool optimistic=false) const override;
			bool IsSubtype(NomTypeVarRef other, bool optimistic=false) const override;
			bool IsSupertype(NomTypeRef other, bool optimistic=false) const override;
			bool IsSupertype(NomBottomTypeRef other, bool optimistic=false) const override;
			bool IsSupertype(NomClassTypeRef other, bool optimistic=false) const override;
			bool IsSupertype(NomTopTypeRef other, bool optimistic=false) const override;
			bool IsSupertype(NomTypeVarRef other, bool optimistic=false) const override;
			virtual NomTypeRef SubstituteSubtyping(const NomSubstitutionContext* context) const override;
			virtual bool ContainsVariables() const override { return false; }

			virtual llvm::Value * GenerateRTInstantiation(NomBuilder &builder, CompileEnv* env) const override;

			virtual llvm::Type * GetLLVMType() const override;

			//virtual const RTTypeBlock GetRTType() const = 0;

			virtual const std::string GetSymbolRep() const override;

			//virtual llvm::Constant *GetTypeTemplate(llvm::Module &mod) const override;
			//virtual llvm::StructType * GetTypeTemplateType() const override;
			//virtual llvm::Constant * GetTypeTemplateReference(uint64_t offset) const override;
			virtual TypeKind GetKind() const override
			{
				return TypeKind::TKBottom;
			}
			//llvm::Constant * GetRTType() const override;
			size_t GetHashCode() const override {
				return 0;
			}

			// Inherited via NomType
			virtual llvm::Constant * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant * findLLVMElement(llvm::Module & mod) const override;

			// Inherited via NomType
			virtual intptr_t GetRTElement() const override;

			// Inherited via NomType
			virtual NomClassTypeRef GetClassInstantiation(const NomNamed * named) const override;

			// Inherited via NomType
			virtual bool IsDisjoint(NomTypeRef other) const override;
			virtual bool IsDisjoint(NomBottomTypeRef other) const override;
			virtual bool IsDisjoint(NomClassTypeRef other) const override;
			virtual bool IsDisjoint(NomTopTypeRef other) const override;
			virtual bool IsDisjoint(NomTypeVarRef other) const override;

			// Inherited via NomType
			virtual bool IsSubtype(NomDynamicTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomDynamicTypeRef other, bool optimistic = false) const override;

			// Inherited via NomType
			virtual bool PossiblyPrimitive() const override;

			// Inherited via NomType
			virtual bool UncertainlyPrimitive() const override;

			// Inherited via NomType
			virtual bool IsSubtype(NomMaybeTypeRef other, bool optimistic = false) const override;
			virtual bool IsSupertype(NomMaybeTypeRef other, bool optimistic = false) const override;
			virtual bool IsDisjoint(NomMaybeTypeRef other) const override;

			// Inherited via NomType
			virtual TypeReferenceType GetTypeReferenceType() const override;
		};

	}
}