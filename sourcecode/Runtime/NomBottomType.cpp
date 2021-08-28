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

		NomBottomType::NomBottomType()
		{
		}


		NomBottomType::~NomBottomType()
		{
		}

		bool NomBottomType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomBottomType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		NomTypeRef NomBottomType::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Value * NomBottomType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			return NomBottomType::GetLLVMElement(*(env->Module));
		}
		llvm::Type * NomBottomType::GetLLVMType() const
		{
			return llvm::StructType::get(LLVMCONTEXT, llvm::ArrayRef<llvm::Type *>({}));
		}
		const std::string NomBottomType::GetSymbolRep() const
		{
			return "Nothing";
		}
		llvm::Constant * NomBottomType::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return new llvm::GlobalVariable(mod, RTTypeHead::GetLLVMType(), true, linkage, RTTypeHead::GetConstant(TypeKind::TKBottom, MakeInt(GetHashCode()), this), "RT_NOM_BottomType");
		}
		llvm::Constant * NomBottomType::findLLVMElement(llvm::Module & mod) const
		{
			return mod.getGlobalVariable("RT_NOM_BottomType");
		}
		intptr_t NomBottomType::GetRTElement() const
		{
			return 0; //TODO: implement
		}
		NomClassTypeRef NomBottomType::GetClassInstantiation(const NomNamed * named) const
		{
			throw new std::exception();
			//return named->GetGeneralBottomType();
		}
		bool NomBottomType::IsDisjoint(NomTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint(NomClassTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint(NomTopTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint(NomTypeVarRef other) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return optimistic;
		}
		bool NomBottomType::PossiblyPrimitive() const
		{
			return false;
		}
		bool NomBottomType::UncertainlyPrimitive() const
		{
			return false;
		}

		bool NomBottomType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomBottomType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return true;
		}
		TypeReferenceType NomBottomType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomBottomType::ContainsVariableIndex(int index) const
		{
			return false;
		}
	}
}