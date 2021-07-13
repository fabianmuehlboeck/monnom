#include "NomTypeVar.h"
#include "NomClassType.h"
#include "RTTypeHead.h"
#include "CompileHelpers.h"
#include "NomTypeParameter.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomMaybeType.h"

namespace Nom
{
	namespace Runtime
	{
		static const std::string gnprefix = "RT_NOM_TVAR_";
		NomTypeVar::NomTypeVar(const NomTypeParameter* ref) : GloballyNamed(&gnprefix), referenceParameter(ref)
		{

		}

		NomTypeRef NomTypeVar::GetUpperBound() const
		{
			return referenceParameter->GetUpperBound();
		}
		NomTypeRef NomTypeVar::GetLowerBound() const
		{
			return referenceParameter->GetLowerBound();
		}
		int NomTypeVar::GetIndex() const
		{
			return referenceParameter->GetIndex();
		}
		const NomTypeParameter* const NomTypeVar::GetParameter() const
		{
			return referenceParameter;
		}

		bool NomTypeVar::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this);
		}
		bool NomTypeVar::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return GetUpperBound()->IsSubtype(other);
		}
		bool NomTypeVar::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return this->GetUpperBound()->IsSubtype(other);
		}
		bool NomTypeVar::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return this->GetIndex() == other->GetIndex() || this->GetUpperBound()->IsSubtype(other->GetLowerBound());
		}
		bool NomTypeVar::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomTypeVar::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return this->GetLowerBound()->IsSupertype(other);
		}
		bool NomTypeVar::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return this->GetLowerBound()->IsSupertype(other);
		}
		bool NomTypeVar::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return this->GetIndex() == other->GetIndex() || this->GetLowerBound()->IsSupertype(other->GetUpperBound());
		}
		NomTypeRef NomTypeVar::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			if (GetIndex() < context->GetTypeArgumentCount())
			{
				return context->GetTypeVariable(GetIndex());
			}
			return this;
		}
		llvm::Type * NomTypeVar::GetLLVMType() const
		{
			return REFTYPE;
		}
		size_t NomTypeVar::GetHashCode() const
		{
			return referenceParameter->GetHashCode();
		}
		llvm::Constant * NomTypeVar::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto var = new llvm::GlobalVariable(mod, RTTypeVar::GetLLVMType(), true, linkage, nullptr, GetGlobalName());
			auto cnst = RTTypeVar::GetConstant(GetIndex(), GetLowerBound()->GetLLVMElement(mod), GetUpperBound()->GetLLVMElement(mod), this);
			var->setInitializer(cnst);
			return llvm::ConstantExpr::getGetElementPtr(cnst->getType(), var, llvm::ArrayRef<llvm::Constant*>({MakeInt32(0), MakeInt32((unsigned char)RTTypeVarFields::Head)}));
		}
		llvm::Constant * NomTypeVar::findLLVMElement(llvm::Module & mod) const
		{
			auto var = mod.getGlobalVariable(GetGlobalName());
			if (var == nullptr)
			{
				return var;
			}
			return llvm::ConstantExpr::getGetElementPtr(var->getType()->getElementType(), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((unsigned char)RTTypeVarFields::Head) }));
		}
		intptr_t NomTypeVar::GetRTElement() const
		{
			return 0;//TODO: implement
		}
		NomClassTypeRef NomTypeVar::GetClassInstantiation(const NomNamed * named) const
		{
			return GetUpperBound()->GetClassInstantiation(named);
		}
		bool NomTypeVar::IsDisjoint(NomTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomTypeVar::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomTypeVar::IsDisjoint(NomClassTypeRef other) const
		{
			return GetUpperBound()->IsDisjoint(other);
		}
		bool NomTypeVar::IsDisjoint(NomTopTypeRef other) const
		{
			return this->GetUpperBound()->IsDisjoint(this);
		}
		bool NomTypeVar::IsDisjoint(NomTypeVarRef other) const
		{
			return GetUpperBound()->IsDisjoint(other->GetUpperBound());
		}
		bool NomTypeVar::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return optimistic || GetLowerBound()->IsSupertype(other);
		}
		bool NomTypeVar::PossiblyPrimitive() const
		{
			return GetUpperBound()->PossiblyPrimitive();
		}
		bool NomTypeVar::UncertainlyPrimitive() const
		{
			auto inttype = NomIntClass::GetInstance()->GetType();
			auto floattype = NomFloatClass::GetInstance()->GetType();
			auto ub = GetUpperBound();
			return !((ub->IsDisjoint(inttype) && ub->IsDisjoint(floattype)) || ub->IsSubtype(inttype,false) || ub->IsSubtype(floattype,false));
		}
		bool NomTypeVar::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTypeVar::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomTypeVar::IsDisjoint(NomMaybeTypeRef other) const
		{
			return GetUpperBound()->IsDisjoint(other);
		}
		TypeReferenceType NomTypeVar::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
	}
}