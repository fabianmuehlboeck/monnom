#include "NomBottomType.h"
#include "NomClassType.h"
#include "NomTopType.h"
#include "NomTypeVar.h"
#include "RTTypeHead.h"
#include "llvm/IR/GlobalVariable.h"
#include "CompileHelpers.h"
#include "NomMaybeType.h"

namespace Nom
{
	namespace Runtime
	{

		NomTopType::NomTopType()
		{
		}


		NomTopType::~NomTopType()
		{
		}

		bool NomTopType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTopType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTopType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomTopType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return true;
		}
		NomTypeRef NomTopType::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Value* NomTopType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			return NomTopType::GetLLVMElement(*(env->Module));
		}
		llvm::Type* NomTopType::GetLLVMType() const
		{
			return llvm::StructType::get(LLVMCONTEXT, llvm::ArrayRef<llvm::Type*>({}));
		}
		const std::string NomTopType::GetSymbolRep() const
		{
			return "Anything";
		}
		llvm::Constant* NomTopType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return new llvm::GlobalVariable(mod, RTTypeHead::GetLLVMType(), true, linkage, RTTypeHead::GetConstant(TypeKind::TKTop, MakeInt(GetHashCode()), this), "RT_NOM_TopType");
		}
		llvm::Constant* NomTopType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_TopType");
		}
		intptr_t NomTopType::GetRTElement() const
		{
			return 0; //TODO: implement
		}
		NomClassTypeRef NomTopType::GetClassInstantiation(const NomNamed* named) const
		{
			throw new std::exception();
		}
		bool NomTopType::IsDisjoint(NomTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomTopType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomTopType::IsDisjoint(NomClassTypeRef other) const
		{
			return false;
		}
		bool NomTopType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomTopType::IsDisjoint(NomTypeVarRef other) const
		{
			return false;
		}
		bool NomTopType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::PossiblyPrimitive() const
		{
			return true;
		}
		bool NomTopType::UncertainlyPrimitive() const
		{
			return true;
		}

		bool NomTopType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomTopType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomTopType::ContainsVariableIndex(int index) const
		{
			return false;
		}
	}
}