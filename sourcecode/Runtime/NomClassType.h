#pragma once
#include "NomType.h"
#include "Defs.h"
#include "TypeList.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/Error.h"
POPDIAGSUPPRESSION
#include "Context.h"
#include "RTClassType.h"
#include "NomAlloc.h"
#include "NomNamed.h"
#include "GloballyNamed.h"
#include "AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClassConstant;
		class NomTypeListConstant;
		class NomNamed;
		class NomClassType;
		class NomMaybeType;
		class NomTopType;
		class NomTypeVar;
		class NomType;
		class NomClassType : public NomType, public GloballyNamed
		{
			friend NomNamed;
		private:
			//mutable const RTClassType * rtclst = nullptr;
			NomClassType(const NomNamed *named, const llvm::ArrayRef<NomTypeRef> args = {});
			const uint64_t id;
			static uint64_t Count() { static uint64_t count = 0; return ++count; }
			mutable RTTypeHead head;
		public:
			static llvm::ArrayRef<NomTypeRef> MeetArguments(const llvm::ArrayRef<NomTypeRef> left, const llvm::ArrayRef<NomTypeRef> right);
			typedef void * (*InitFunctionPointer)(void *, void *, size_t, const NomType *, int, const NomTypeRef *);
			static InitFunctionPointer GetCPPInitializerFunction();
			static llvm::Function *GetInitializerFunction(llvm::Module &mod);

			static bool ArgumentsSubtypes(const TypeList & left, const TypeList & right, bool optimistic=false);

			const NomNamed * const Named;
			const llvm::ArrayRef<NomTypeRef> Arguments;
			virtual ~NomClassType() override
			{
				if (Arguments.data() != nullptr)
				{
					nfree(const_cast<NomTypeRef *>(Arguments.data()));
				}
			}

			bool IsSubtype(NomTypeRef other, bool optimistic = false) const override;
			bool IsSubtype(NomBottomTypeRef other, bool optimistic = false) const override;
			bool IsSubtype(NomClassTypeRef other, bool optimistic = false) const override;
			bool IsSubtype(NomTopTypeRef other, bool optimistic = false) const override;
			bool IsSubtype(NomTypeVarRef other, bool optimistic = false) const override;
			bool IsSupertype(NomTypeRef other, bool optimistic = false) const override;
			bool IsSupertype(NomBottomTypeRef other, bool optimistic = false) const override;
			bool IsSupertype(NomClassTypeRef other, bool optimistic = false) const override;
			bool IsSupertype(NomTopTypeRef other, bool optimistic = false) const override;
			bool IsSupertype(NomTypeVarRef other, bool optimistic = false) const override;
			virtual NomTypeRef SubstituteSubtyping(const NomSubstitutionContext* context) const override;
			virtual bool ContainsVariables() const override;

			size_t GetHashCode() const override;

			static size_t GetHashCode(const NomNamed* named, size_t typeArrHash);

			llvm::Type * GetLLVMType() const override;
			const std::string GetSymbolRep() const override;
		private:
			mutable llvm::Constant * typeTemplate = nullptr;
			mutable llvm::StructType * typeTemplateType = nullptr;

		public:
			virtual TypeKind GetKind() const override
			{
				return TypeKind::TKClass;
			}

			// Inherited via NomType
			virtual llvm::Value * GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const override;

			// Inherited via NomType
			virtual llvm::Constant * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant * findLLVMElement(llvm::Module & mod) const override;
			virtual uintptr_t GetRTElement() const override;

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
			virtual bool ContainsVariableIndex(size_t index) const override;
		};
	}
}

extern "C" uintptr_t NOM_RTInstantiateClass(intptr_t cls, intptr_t *args);
