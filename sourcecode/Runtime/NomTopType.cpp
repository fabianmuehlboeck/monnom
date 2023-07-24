#include "NomBottomType.h"
#include "NomClassType.h"
#include "NomTopType.h"
#include "NomTypeVar.h"
#include "RTTypeHead.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/GlobalVariable.h"
POPDIAGSUPPRESSION
#include "CompileHelpers.h"
#include "NomMaybeType.h"
#include "CallingConvConf.h"

using namespace llvm;
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

		bool NomTopType::IsSubtype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTopType::IsSubtype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSubtype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSubtype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSubtype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		NomTypeRef NomTopType::SubstituteSubtyping([[maybe_unused]] const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Value* NomTopType::GenerateRTInstantiation([[maybe_unused]] NomBuilder& builder, CompileEnv* env) const
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
			Function* fun = Function::Create(GetCastFunctionType(), linkage, "MONNOM_RT_TYPECASTFUN_TOP", mod);
			{
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				builder->SetInsertPoint(startBlock);
				auto argiter = fun->arg_begin();
				argiter++;
				builder->CreateRet(argiter);
			}
			return new llvm::GlobalVariable(mod, RTTypeHead::GetLLVMType(), true, linkage, RTTypeHead::GetConstant(TypeKind::TKTop, MakeInt(GetHashCode()), this, fun), "RT_NOM_TopType");
		}
		llvm::Constant* NomTopType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_TopType");
		}
		uintptr_t NomTopType::GetRTElement() const
		{
			return 0; //TODO: implement
		}
		NomClassTypeRef NomTopType::GetClassInstantiation([[maybe_unused]] const NomNamed* named) const
		{
			throw new std::exception();
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomClassTypeRef other) const
		{
			return false;
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomTopTypeRef other) const
		{
			return false;
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomTypeVarRef other) const
		{
			return false;
		}
		bool NomTopType::IsSubtype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
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

		bool NomTopType::IsSubtype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return false;
		}
		bool NomTopType::IsSupertype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomTopType::IsDisjoint([[maybe_unused]] NomMaybeTypeRef other) const
		{
			return false;
		}
		TypeReferenceType NomTopType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomTopType::ContainsVariableIndex([[maybe_unused]] size_t index) const
		{
			return false;
		}
	}
}
