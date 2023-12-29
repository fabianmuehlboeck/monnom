#include "NomDynamicType.h"
#include "NomTypeVar.h"
#include "NomClassType.h"
#include "NomTopType.h"
#include "NomBottomType.h"
#include "XRTDynamicType.h"
#include "CompileHelpers.h"
#include "NomMaybeType.h"
#include "RTTypeHead.h"
#include "CallingConvConf.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		NomDynamicType::NomDynamicType(TypeKind _kind) : kind(_kind)
		{
		}
		NomDynamicType& NomDynamicType::Instance()
		{
			[[clang::no_destroy]] static NomDynamicType ndt(TypeKind::TKDynamic); return ndt;
		}
		NomDynamicType& NomDynamicType::LambdaInstance()
		{
			[[clang::no_destroy]] static NomDynamicType ndt(TypeKind::TKLambda); return ndt;
		}
		NomDynamicType& NomDynamicType::PartialAppInstance()
		{
			[[clang::no_destroy]] static NomDynamicType ndt(TypeKind::TKPartialApp); return ndt;
		}
		NomDynamicType& NomDynamicType::RecordInstance()
		{
			[[clang::no_destroy]] static NomDynamicType ndt(TypeKind::TKRecord); return ndt;
		}
		Constant* NomDynamicType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetCastFunctionType(), linkage, "MONNOM_RT_TYPECASTFUN_DYNAMIC", mod);
			{
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				builder->SetInsertPoint(startBlock);
				auto argiter = fun->arg_begin();
				argiter++;
				builder->CreateRet(argiter);
			}
			auto gv = new GlobalVariable(mod, XRTDynamicType::GetLLVMType(), true, linkage, XRTDynamicType::CreateConstant(fun), "RT_NOM_DynamicType");
			return llvm::ConstantExpr::getGetElementPtr(XRTDynamicType::GetLLVMType(), gv, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((XRTDynamicTypeFields::Head)) }));
		}
		Constant* NomDynamicType::findLLVMElement(llvm::Module& mod) const
		{
			auto gv = mod.getGlobalVariable("RT_NOM_DynamicType");
			if (gv == nullptr)
			{
				return gv;
			}
			return llvm::ConstantExpr::getGetElementPtr(XRTDynamicType::GetLLVMType(), gv, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((XRTDynamicTypeFields::Head)) }));
		}
		bool NomDynamicType::ContainsVariables() const
		{
			return false;
		}
		llvm::Value* NomDynamicType::GenerateRTInstantiation([[maybe_unused]] NomBuilder& builder, CompileEnv* env) const
		{
			return GetLLVMElement(*env->Module);
		}
		size_t NomDynamicType::GetHashCode() const
		{
			return static_cast<size_t>(reinterpret_cast<intptr_t>(this));
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return optimistic;
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return optimistic;
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return  other->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomClassTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomTopTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomTypeVarRef other) const
		{
			return false;
		}
		NomTypeRef NomDynamicType::SubstituteSubtyping([[maybe_unused]] const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Type* NomDynamicType::GetLLVMType() const
		{
			return REFTYPE;
		}
		const std::string NomDynamicType::GetSymbolRep() const
		{
			return std::string("$DYN");
		}
		TypeKind NomDynamicType::GetKind() const
		{
			return kind;
		}
		uintptr_t NomDynamicType::GetRTElement() const
		{
			return uintptr_t();
		}
		NomClassTypeRef NomDynamicType::GetClassInstantiation([[maybe_unused]] const NomNamed* named) const
		{
			return nullptr;
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::PossiblyPrimitive() const
		{
			return true;
		}
		bool NomDynamicType::UncertainlyPrimitive() const
		{
			return false;
		}
		bool NomDynamicType::IsSubtype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return optimistic || other->PotentialType->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSupertype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint([[maybe_unused]] NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomDynamicType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomDynamicType::ContainsVariableIndex([[maybe_unused]] size_t index) const
		{
			return false;
		}
	}
}
