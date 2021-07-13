#include "NomPartialAppType.h"
#include "NomType.h"
#include "RTTypeHead.h"
#include "RTPartialAppType.h"
#include "CompileHelpers.h"
#include "NomTopType.h"
#include "NomClassType.h"
#include "NomTypeVar.h"
#include "NomBottomType.h"
#include "NomDynamicType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomPartialAppType& NomPartialAppType::Instance()
		{
			static NomPartialAppType npat; return npat;
		}
		llvm::Constant* NomPartialAppType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return new GlobalVariable(mod, RTPartialAppType::GetLLVMType(), true, linkage, ConstantStruct::get(RTPartialAppType::GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKPartialApp, MakeInt(GetHashCode()), this)), "RT_NOM_PartialAppType");
		}
		llvm::Constant* NomPartialAppType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_PartialAppType");
		}
		bool NomPartialAppType::ContainsVariables() const
		{
			return false;
		}
		size_t NomPartialAppType::GetHashCode() const
		{
			return (size_t)(intptr_t)this;
		}
		bool NomPartialAppType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomPartialAppType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return other->GetLowerBound()->IsSupertype(this);
		}
		bool NomPartialAppType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomPartialAppType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsDisjoint(NomTypeRef other) const
		{
			return false;
		}
		bool NomPartialAppType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomPartialAppType::IsDisjoint(NomClassTypeRef other) const
		{
			return true;
		}
		bool NomPartialAppType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomPartialAppType::IsDisjoint(NomTypeVarRef other) const
		{
			return true;
		}
		NomTypeRef NomPartialAppType::SubstituteSubtyping(const NomSubstitutionContext *context) const
		{
			return this;
		}
		llvm::Type* NomPartialAppType::GetLLVMType() const
		{
			return REFTYPE;
		}
		const std::string NomPartialAppType::GetSymbolRep() const
		{
			return std::string();
		}
		TypeKind NomPartialAppType::GetKind() const
		{
			return TypeKind::TKPartialApp;
		}
		intptr_t NomPartialAppType::GetRTElement() const
		{
			return intptr_t();
		}
		NomClassTypeRef NomPartialAppType::GetClassInstantiation(const NomNamed* named) const
		{
			return NomClassTypeRef();
		}
		llvm::Value* NomPartialAppType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			throw new std::exception();
		}
		bool NomPartialAppType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomPartialAppType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomPartialAppType::PossiblyPrimitive() const
		{
			return false;
		}
		bool NomPartialAppType::UncertainlyPrimitive() const
		{
			return false;
		}
		bool NomPartialAppType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomPartialAppType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomPartialAppType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
	}
}