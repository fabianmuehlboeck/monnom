#include "NomBottomType.h"
#include "NomClassType.h"
#include "NomTopType.h"
#include "NomTypeVar.h"
#include "RTTypeHead.h"
#include "llvm/IR/GlobalVariable.h"
#include "CompileHelpers.h"
#include "NomMaybeType.h"
#include "CallingConvConf.h"
#include "RTOutput.h"

using namespace llvm;
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

		bool NomBottomType::IsSubtype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomBottomTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomClassTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomTopTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return false;
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomTypeVarRef other, [[maybe_unused]] bool optimistic) const
		{
			return other->IsSubtype(this);
		}
		NomTypeRef NomBottomType::SubstituteSubtyping([[maybe_unused]] const NomSubstitutionContext* context) const
		{
			return this;
		}
		llvm::Value* NomBottomType::GenerateRTInstantiation([[maybe_unused]] NomBuilder& builder, [[maybe_unused]] CompileEnv* env) const
		{
			return NomBottomType::GetLLVMElement(*(env->Module));
		}
		llvm::Type* NomBottomType::GetLLVMType() const
		{
			return llvm::StructType::get(LLVMCONTEXT, llvm::ArrayRef<llvm::Type*>({}));
		}
		const std::string NomBottomType::GetSymbolRep() const
		{
			return "Nothing";
		}
		llvm::Constant* NomBottomType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetCastFunctionType(), linkage, "MONNOM_RT_TYPECASTFUN_BOTTOM", mod);
			{
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cast failed: value cannot be subtype of bottom!", startBlock);
			}
			return new llvm::GlobalVariable(mod, RTTypeHead::GetLLVMType(), true, linkage, RTTypeHead::GetConstant(TypeKind::TKBottom, MakeInt(GetHashCode()), this, fun), "RT_NOM_BottomType");
		}
		llvm::Constant* NomBottomType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_BottomType");
		}
		uintptr_t NomBottomType::GetRTElement() const
		{
			return 0; //TODO: implement
		}
		NomClassTypeRef NomBottomType::GetClassInstantiation([[maybe_unused]] const NomNamed* named) const
		{
			throw new std::exception();
			//return named->GetGeneralBottomType();
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomClassTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomTopTypeRef other) const
		{
			return true;
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomTypeVarRef other) const
		{
			return true;
		}
		bool NomBottomType::IsSubtype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomDynamicTypeRef other, [[maybe_unused]] bool optimistic) const
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

		bool NomBottomType::IsSubtype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return true;
		}
		bool NomBottomType::IsSupertype([[maybe_unused]] NomMaybeTypeRef other, [[maybe_unused]] bool optimistic) const
		{
			return false;
		}
		bool NomBottomType::IsDisjoint([[maybe_unused]] NomMaybeTypeRef other) const
		{
			return true;
		}
		TypeReferenceType NomBottomType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomBottomType::ContainsVariableIndex([[maybe_unused]] size_t index) const
		{
			return false;
		}
	}
}
