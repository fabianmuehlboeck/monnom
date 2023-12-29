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
		[[clang::no_destroy]] static const std::string gnprefix = "RT_NOM_TVAR_";
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
		size_t NomTypeVar::GetIndex() const
		{
			return referenceParameter->GetIndex();
		}
		const NomTypeParameter* NomTypeVar::GetParameter() const
		{
			return referenceParameter;
		}

		bool NomTypeVar::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTypeVar::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return GetUpperBound()->IsSubtype(other, optimistic);
		}
		bool NomTypeVar::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return this->GetUpperBound()->IsSubtype(other, optimistic);
		}
		bool NomTypeVar::IsSubtype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return this->GetIndex() == other->GetIndex() || this->GetUpperBound()->IsSubtype(other->GetLowerBound(), optimistic);
		}
		bool NomTypeVar::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomTypeVar::IsSupertype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return this->GetLowerBound()->IsSupertype(other, optimistic);
		}
		bool NomTypeVar::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return this->GetLowerBound()->IsSupertype(other, optimistic);
		}
		bool NomTypeVar::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return this->GetIndex() == other->GetIndex() || this->GetLowerBound()->IsSupertype(other->GetUpperBound(), optimistic);
		}
		NomTypeRef NomTypeVar::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			if (GetIndex() < context->GetTypeArgumentCount())
			{
				auto replacement = context->GetTypeVariable(GetIndex());
				if (replacement != nullptr)
				{
					return replacement;
				}
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
			auto var = new llvm::GlobalVariable(mod, XRTTypeVar::GetLLVMType(), true, linkage, nullptr, GetGlobalName());
			auto cnst = XRTTypeVar::GetConstant(GetIndex(), GetLowerBound()->GetLLVMElement(mod), GetUpperBound()->GetLLVMElement(mod), this);
			var->setInitializer(cnst);
			return llvm::ConstantExpr::getGetElementPtr(cnst->getType(), var, llvm::ArrayRef<llvm::Constant*>({MakeInt32(0), MakeInt32((XRTTypeVarFields::Head))}));
		}
		llvm::Constant * NomTypeVar::findLLVMElement(llvm::Module & mod) const
		{
			auto var = mod.getGlobalVariable(GetGlobalName());
			if (var == nullptr)
			{
				return var;
			}
			return llvm::ConstantExpr::getGetElementPtr(XRTTypeVar::GetLLVMType(), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((XRTTypeVarFields::Head)) }));
		}
		uintptr_t NomTypeVar::GetRTElement() const
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
		bool NomTypeVar::IsDisjoint([[maybe_unused]] NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomTypeVar::IsDisjoint(NomClassTypeRef other) const
		{
			NomSingleSubstitutionContext nssc = NomSingleSubstitutionContext(this, GetDynamicType());
			return GetUpperBound()->SubstituteSubtyping(&nssc)->IsDisjoint(other);
		}
		bool NomTypeVar::IsDisjoint(NomTopTypeRef other) const
		{
			return this->GetUpperBound()->IsDisjoint(other);
		}
		bool NomTypeVar::IsDisjoint(NomTypeVarRef other) const
		{
			NomSingleSubstitutionContext nssc = NomSingleSubstitutionContext(this, GetDynamicType());
			return other->IsDisjoint(GetUpperBound()->SubstituteSubtyping(&nssc));
		}
		bool NomTypeVar::IsSubtype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTypeVar::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return optimistic || GetLowerBound()->IsSupertype(other, optimistic);
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
			NomSingleSubstitutionContext nssc = NomSingleSubstitutionContext(this, GetDynamicType());
			return GetUpperBound()->SubstituteSubtyping(&nssc)->IsDisjoint(other);
		}
		TypeReferenceType NomTypeVar::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomTypeVar::ContainsVariableIndex(size_t index) const
		{
			return this->GetIndex()==index;
		}
	}
}
