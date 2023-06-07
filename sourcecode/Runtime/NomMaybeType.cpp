#include "NomMaybeType.h"
#include "NomBottomType.h"
#include "NomClassType.h"
#include "NomTypeVar.h"
#include "NullClass.h"
#include "NomTypeRegistry.h"
#include <unordered_map>
#include "CompileHelpers.h"
#include "Defs.h"
#include "RTMaybeType.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{
		static const std::string nomMaybeTypePrefix = "RT_NOM_MAYBETYPE_";
		NomMaybeType::NomMaybeType(NomTypeRef potentialType) : GloballyNamed(&nomMaybeTypePrefix), PotentialType(potentialType)
		{
		}
		NomMaybeTypeRef NomMaybeType::GetMaybeType(NomTypeRef potentialType)
		{
			static std::unordered_map<NomTypeRef, NomMaybeTypeRef> instances;
			if (potentialType->GetKind() == TypeKind::TKMaybe)
			{
				return (NomMaybeTypeRef)potentialType;
			}
			auto result = instances.find(potentialType);
			if (result == instances.end())
			{
				auto mbt = new NomMaybeType(potentialType);
				instances[potentialType] = mbt;
				return mbt;
			}
			return instances[potentialType];
		}
		NomMaybeType::~NomMaybeType()
		{
		}
		llvm::Constant* NomMaybeType::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::StructType* ttt = RTMaybeType::GetLLVMType();
			llvm::GlobalVariable* gv = new llvm::GlobalVariable(mod, ttt, true, linkage, nullptr, GetGlobalName());
			gv->setInitializer(RTMaybeType::GetConstant(mod, this));
			return llvm::ConstantExpr::getGetElementPtr(ttt, gv, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head) }));
		}
		llvm::Constant* NomMaybeType::findLLVMElement(llvm::Module& mod) const
		{
			llvm::SmallVector<char, 32> buf;
			auto var = mod.getGlobalVariable(GetGlobalName());
			if (var == nullptr)
			{
				return var;
			}
			return llvm::ConstantExpr::getGetElementPtr(RTMaybeType::GetLLVMType(), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head) }));
		}
		bool NomMaybeType::ContainsVariables() const
		{
			return PotentialType->ContainsVariables();
		}
		llvm::Value* NomMaybeType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			if (PotentialType->ContainsVariables())
			{
				llvm::Value* argarr[1];
				argarr[0] = this->PotentialType->GenerateRTInstantiation(builder, env);
				return builder->CreateCall(env->Module->getFunction("NOM_RTInstantiateMaybe"), argarr);
			}
			else
			{
				return this->GetLLVMElement(*(env->Module));
			}
		}
		size_t NomMaybeType::GetHashCode() const
		{
			return (17 * 23 + PotentialType->GetHashCode())*23;
		}
		bool NomMaybeType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomMaybeType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomMaybeType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomMaybeType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			return PotentialType->IsSubtype(other, optimistic)&&NomNullClass::GetInstance()->GetType()->IsSubtype(other);
		}
		bool NomMaybeType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomMaybeType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return IsSubtype(other->GetLowerBound(), optimistic);
		}
		bool NomMaybeType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this,optimistic);
		}
		bool NomMaybeType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return optimistic;
		}
		bool NomMaybeType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomMaybeType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return PotentialType->IsSupertype(other) || NomNullClass::GetInstance()->GetType()->IsSupertype(other);
		}
		bool NomMaybeType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomMaybeType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return PotentialType->IsSupertype(other) || NomNullClass::GetInstance()->GetType()->IsSupertype(other);
		}
		bool NomMaybeType::IsDisjoint(NomTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomMaybeType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomMaybeType::IsDisjoint(NomClassTypeRef other) const
		{
			return PotentialType->IsDisjoint(other) && NomNullClass::GetInstance()->GetType()->IsDisjoint(other);
		}
		bool NomMaybeType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomMaybeType::IsDisjoint(NomTypeVarRef other) const
		{
			return PotentialType->IsDisjoint(other) && NomNullClass::GetInstance()->GetType()->IsDisjoint(other);
		}
		bool NomMaybeType::PossiblyPrimitive() const
		{
			return PotentialType->PossiblyPrimitive();
		}
		bool NomMaybeType::UncertainlyPrimitive() const
		{
			return PotentialType->UncertainlyPrimitive();
		}
		NomTypeRef NomMaybeType::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			return NomMaybeType::GetMaybeType(PotentialType->SubstituteSubtyping(context));
		}
		llvm::Type* NomMaybeType::GetLLVMType() const
		{
			return REFTYPE;
		}
		const std::string NomMaybeType::GetSymbolRep() const
		{
			return "RT_NOM_MAYBE$$" + PotentialType->GetSymbolRep();
		}
		TypeKind NomMaybeType::GetKind() const
		{
			return TypeKind::TKMaybe;
		}
		intptr_t NomMaybeType::GetRTElement() const
		{
			if (head.Entry() != nullptr)
			{
				return reinterpret_cast<intptr_t>(head.Entry());
			}
			if (auto var = (intptr_t)(void*)NomJIT::Instance().lookup(GetGlobalName())->getValue())
			{
				intptr_t ret = (var)+RTMaybeType::HeadOffset();
				head = RTTypeHead((void*)ret);
				return ret;
			}
			auto mmod = GetMainModule();
			if (mmod != nullptr)
			{
				throw new std::exception();
			}
			void* rtct = (nmalloc(RTMaybeType::SizeOf()));
			auto initfun = GetCPPInitializerFunction();
			return (intptr_t)initfun(rtct, (void*)this->PotentialType->GetRTElement(), this->GetHashCode(), this);
		}
		NomClassTypeRef NomMaybeType::GetClassInstantiation(const NomNamed* named) const
		{
			throw new std::exception();
		}
		bool NomMaybeType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return PotentialType->IsSubtype(other,optimistic);
		}
		bool NomMaybeType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return PotentialType->IsSupertype(other, optimistic);
		}
		bool NomMaybeType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return false;
		}
		NomMaybeType::InitFunctionPointer NomMaybeType::GetCPPInitializerFunction()
		{
			return (InitFunctionPointer)((intptr_t)(void*)(NomJIT::Instance().lookup("RT_NOM_MaybeTypeInitializer")->getValue()));
		}
		llvm::Function* NomMaybeType::GetInitializerFunction(llvm::Module& mod)
		{
			static Module* initializerModule = &mod;
			Function* fun = mod.getFunction("RT_NOM_MaybeTypeInitializer");
			if (fun == nullptr)
			{
				FunctionType* funtype = llvm::FunctionType::get(RTTypeHead::GetLLVMType()->getPointerTo(), { RTMaybeType::GetLLVMType()->getPointerTo(), RTTypeHead::GetLLVMType()->getPointerTo(), numtype(size_t), POINTERTYPE }, false);
				fun = llvm::Function::Create(funtype, (&mod == initializerModule) ? llvm::GlobalValue::LinkageTypes::ExternalLinkage : llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage, "RT_NOM_MaybeTypeInitializer", &mod);
				NomBuilder builder;
				auto argiter = fun->arg_begin();
				Value* mbtypepointer = argiter;
				Value* ptypepointer = argiter + 1;
				Value* hash = argiter + 2;
				Value* nomtype = argiter + 3;

				BasicBlock* mainBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

				builder->SetInsertPoint(mainBlock);
				StoreInst* store = MakeStore(builder, mod, MakeInt((unsigned char)TypeKind::TKClass), builder->CreateGEP(RTMaybeType::GetLLVMType(), mbtypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::Kind) }));
				store = MakeStore(builder, mod, hash, builder->CreateGEP(RTMaybeType::GetLLVMType(), mbtypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::Hash) }));
				store = MakeStore(builder, mod, nomtype, builder->CreateGEP(RTMaybeType::GetLLVMType(), mbtypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::NomType) }));
				store = MakeStore(builder, mod, ptypepointer, builder->CreateGEP(RTMaybeType::GetLLVMType(), mbtypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::PotentialType) }));

				builder->CreateRet(builder->CreateGEP(RTMaybeType::GetLLVMType(), mbtypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTMaybeTypeFields::Head) }));
			}
			return fun;
		}
		TypeReferenceType NomMaybeType::GetTypeReferenceType() const
		{
			return TypeReferenceType::Reference;
		}
		bool NomMaybeType::ContainsVariableIndex(int index) const
		{
			return PotentialType->ContainsVariableIndex(index);
		}
	}
}

using namespace Nom::Runtime;

extern "C" intptr_t NOM_RTInstantiateMaybe(intptr_t ptype)
{
	NomTypeRef tp = reinterpret_cast<NomTypeRef>(ptype);
	return NomMaybeType::GetMaybeType(tp)->GetRTElement();
}