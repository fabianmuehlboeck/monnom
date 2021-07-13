#include "NomStructType.h"
#include "RTTypeHead.h"
#include "RTStructType.h"
#include "CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomStructType& NomStructType::Instance()
		{
			static NomStructType nst; return nst;
		}
		llvm::Constant* NomStructType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return new GlobalVariable(mod, RTStructType::GetLLVMType(), true, linkage, llvm::ConstantStruct::get(RTStructType::GetLLVMType(), RTTypeHead::GetConstant(TypeKind::TKDynamic, MakeInt(GetHashCode()), this)), "RT_NOM_StructType");
		}
		llvm::Constant* NomStructType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_StructType");
		}
		bool NomStructType::ContainsVariables() const
		{
			return false;
		}
		size_t NomStructType::GetHashCode() const
		{
			return (size_t)(intptr_t)this;
		}

		bool NomStructType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomStructType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomStructType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomStructType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomStructType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsDisjoint(NomTypeRef other) const
		{
			return false;
		}
		bool NomStructType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomStructType::IsDisjoint(NomClassTypeRef other) const
		{
			return false;
		}
		bool NomStructType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomStructType::IsDisjoint(NomTypeVarRef other) const
		{
			return false;
		}
		NomTypeRef NomStructType::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Type* NomStructType::GetLLVMType() const
		{
			return REFTYPE;
		}
		const std::string NomStructType::GetSymbolRep() const
		{
			return std::string();
		}
		TypeKind NomStructType::GetKind() const
		{
			return TypeKind::TKDynamic;
		}
		intptr_t NomStructType::GetRTElement() const
		{
			return intptr_t();
		}
		NomClassTypeRef NomStructType::GetClassInstantiation(const NomNamed* named) const
		{
			return NomClassTypeRef();
		}
		llvm::Value* NomStructType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			throw new std::exception();
		}
		bool NomStructType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomStructType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomStructType::PossiblyPrimitive() const
		{
			return false;
		}
		bool NomStructType::UncertainlyPrimitive() const
		{
			return false;
		}
		bool NomStructType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomStructType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomStructType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
	}
}