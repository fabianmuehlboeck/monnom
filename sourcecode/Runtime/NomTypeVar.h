#pragma once
#include "NomType.h"
#include "RTTypeVar.h"
#include "GloballyNamed.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class NomTypeParameter;
		class NomTypeVar : public NomType, public GloballyNamed
		{
		public:
			const NomTypeParameter * const referenceParameter;

			NomTypeVar(const NomTypeParameter* ref);


			NomTypeRef GetUpperBound() const;
			NomTypeRef GetLowerBound() const;
			int GetIndex() const;
			const NomTypeParameter* const GetParameter() const;

			~NomTypeVar()
			{

			}
			bool IsSubtype(NomTypeRef other, bool optimistic = true) const override;
			bool IsSubtype(NomBottomTypeRef other, bool optimistic = true) const override;
			bool IsSubtype(NomClassTypeRef other, bool optimistic = true) const override;
			bool IsSubtype(NomTopTypeRef other, bool optimistic = true) const override;
			bool IsSubtype(NomTypeVarRef other, bool optimistic = true) const override;
			bool IsSupertype(NomTypeRef other, bool optimistic = true) const override;
			bool IsSupertype(NomBottomTypeRef other, bool optimistic = true) const override;
			bool IsSupertype(NomClassTypeRef other, bool optimistic = true) const override;
			bool IsSupertype(NomTopTypeRef other, bool optimistic = true) const override;
			bool IsSupertype(NomTypeVarRef other, bool optimistic = true) const override;


			virtual llvm::Value * GenerateRTInstantiation(NomBuilder &builder, CompileEnv* env) const override 
			{
				return env->GetTypeArgument(builder, this->GetIndex());
			}
			virtual bool ContainsVariables() const override { return true; }
			NomTypeRef SubstituteSubtyping(const NomSubstitutionContext* context) const override;
			llvm::Type * GetLLVMType() const override;
			const std::string GetSymbolRep() const override
			{
				return "VAR";
			}
			size_t GetHashCode() const override;

		public:
			virtual TypeKind GetKind() const override
			{
				return TypeKind::TKVariable;
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

			// Inherited via NomType
			virtual bool ContainsVariableIndex(int index) const override;
		};
	}
}


