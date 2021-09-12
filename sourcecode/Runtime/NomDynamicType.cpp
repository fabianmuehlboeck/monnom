#include "NomDynamicType.h"
#include "NomTypeVar.h"
#include "NomClassType.h"
#include "NomTopType.h"
#include "NomBottomType.h"
#include "RTDynamicType.h"
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
		NomDynamicType::NomDynamicType(TypeKind kind) : kind(kind)
		{
		}
		NomDynamicType& NomDynamicType::Instance()
		{
			static NomDynamicType ndt(TypeKind::TKDynamic); return ndt;
		}
		NomDynamicType& NomDynamicType::LambdaInstance()
		{
			static NomDynamicType ndt(TypeKind::TKLambda); return ndt;
		}
		NomDynamicType& NomDynamicType::PartialAppInstance()
		{
			static NomDynamicType ndt(TypeKind::TKPartialApp); return ndt;
		}
		NomDynamicType& NomDynamicType::RecordInstance()
		{
			static NomDynamicType ndt(TypeKind::TKRecord); return ndt;
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
			auto gv = new GlobalVariable(mod, RTDynamicType::GetLLVMType(), true, linkage, RTDynamicType::CreateConstant(fun), "RT_NOM_DynamicType");
			return llvm::ConstantExpr::getGetElementPtr(gv->getType()->getElementType(), gv, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((unsigned char)RTDynamicTypeFields::Head) }));
		}
		Constant* NomDynamicType::findLLVMElement(llvm::Module& mod) const
		{
			auto gv = mod.getGlobalVariable("RT_NOM_DynamicType");
			if (gv == nullptr)
			{
				return gv;
			}
			return llvm::ConstantExpr::getGetElementPtr(gv->getType()->getElementType(), gv, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((unsigned char)RTDynamicTypeFields::Head) }));
		}
		bool NomDynamicType::ContainsVariables() const
		{
			return false;
		}
		llvm::Value* NomDynamicType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			return GetLLVMElement(*env->Module);
		}
		size_t NomDynamicType::GetHashCode() const
		{
			return (size_t)(intptr_t)this;
		}
		bool NomDynamicType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return optimistic;
		}
		bool NomDynamicType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return optimistic;
		}
		bool NomDynamicType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return  other->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint(NomTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint(NomClassTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomDynamicType::IsDisjoint(NomTypeVarRef other) const
		{
			return false;
		}
		NomTypeRef NomDynamicType::SubstituteSubtyping(const NomSubstitutionContext* context) const
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
		intptr_t NomDynamicType::GetRTElement() const
		{
			return intptr_t();
		}
		NomClassTypeRef NomDynamicType::GetClassInstantiation(const NomNamed* named) const
		{
			return nullptr;
		}
		bool NomDynamicType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
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
		bool NomDynamicType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return optimistic || other->PotentialType->IsSupertype(this, optimistic);
		}
		bool NomDynamicType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomDynamicType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomDynamicType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomDynamicType::ContainsVariableIndex(int index) const
		{
			return false;
		}
	}
}