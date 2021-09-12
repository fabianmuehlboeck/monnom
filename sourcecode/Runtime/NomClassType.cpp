#include "NomClassType.h"
#include "NomTopType.h"
#include "NomConstants.h"
#include "NomTypeVar.h"
#include "NomClass.h"
#include "NomAlloc.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomJIT.h"
#include "NomBottomType.h"
#include "Util.h"
#include "RTClassType.h"
#include "CompileHelpers.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomMaybeType.h"
#include "BoolClass.h"
#include "RTInterface.h"
#include "BoehmContainers.h"
#include "CallingConvConf.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "RTClass.h"
#include "RTTypeEq.h"
#include "RTSubstStack.h"
#include "RTOutput.h"
#include "Metadata.h"
#include "StructuralValueHeader.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::ArrayRef<NomTypeRef> NomClassType::MeetArguments(const llvm::ArrayRef<NomTypeRef> left, const llvm::ArrayRef<NomTypeRef> right)
		{
			size_t size = left.size();
			if (size != right.size())
			{
				throw new std::exception();
			}
			BoehmVector<NomTypeRef> *vec = new BoehmVector<NomTypeRef>(size);
			for (size_t i = 0; i < size; i++)
			{
				if (!left[i]->IsSubtype(right[i]))
				{
					throw new std::exception();
				}
				if (!right[i]->IsSubtype(left[i]))
				{
					throw new std::exception();
				}
				vec->operator[](i) = left[i];
				//vec->operator[](i) = left[i]->Meet(right[i]);
				//i++;
				//vec->operator[](i) = left[i]->Join(right[i]);
			}
			return *vec;
		}
		NomClassType::InitFunctionPointer NomClassType::GetCPPInitializerFunction()
		{
			return (InitFunctionPointer)((intptr_t)(NomJIT::Instance().lookup("RT_NOM_ClassTypeInitializer")->getAddress()));
		}
		llvm::Function * NomClassType::GetInitializerFunction(llvm::Module & mod)
		{
			static Module *initializerModule = &mod;
			Function *fun = mod.getFunction("RT_NOM_ClassTypeInitializer");
			if (fun == nullptr)
			{
				FunctionType *funtype = llvm::FunctionType::get(RTTypeHead::GetLLVMType()->getPointerTo(), {RTClassType::GetLLVMType()->getPointerTo(), RTClass::GetLLVMType()->getPointerTo(), numtype(size_t), POINTERTYPE, numtype(int), RTTypeHead::GetLLVMType()->getPointerTo()->getPointerTo()}, false);
				fun = llvm::Function::Create(funtype, (&mod == initializerModule) ? llvm::GlobalValue::LinkageTypes::ExternalLinkage : llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage, "RT_NOM_ClassTypeInitializer", &mod);
				NomBuilder builder;
				auto argiter = fun->arg_begin();
				Value *ctypepointer = argiter;
				Value *clspointer = argiter+1;
				Value *hash = argiter + 2;
				Value *nomtype = argiter+3;
				Value *argcount = argiter+4;
				Value *argsarr = argiter+5;

				BasicBlock *mainBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock *loopHead = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock *loop = BasicBlock::Create(LLVMCONTEXT, "", fun);
				BasicBlock *retBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);

				builder->SetInsertPoint(mainBlock);
				StoreInst *store = MakeStore(builder,mod, MakeInt((unsigned char)TypeKind::TKClass), builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::Kind) }));
				store = MakeStore(builder,mod, hash, builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::Hash) }));
				store = MakeStore(builder,mod, nomtype, builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head), MakeInt32((unsigned char)RTTypeHeadFields::NomType) }));
				store = MakeStore(builder,mod, builder->CreatePointerCast(clspointer, RTInterface::GetLLVMType()->getPointerTo()),  builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Class) }));
				builder->CreateBr(loopHead);

				builder->SetInsertPoint(loopHead);
				PHINode *phi = builder->CreatePHI(numtype(int), 2);
				phi->addIncoming(argcount, mainBlock);
				builder->CreateCondBr(builder->CreateICmpEQ(phi, MakeInt<int>(0)), retBlock, loop);

				builder->SetInsertPoint(loop);
				Value *index = builder->CreateSub(phi, MakeInt<int>(1));
				phi->addIncoming(index, loop);
				LoadInst *load = MakeLoad(builder,mod,builder->CreateGEP(argsarr, index));
				store = MakeStore(builder,mod, load, builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::TypeArgs), builder->CreateNeg(phi) }));
				builder->CreateBr(loopHead);

				builder->SetInsertPoint(retBlock);
				builder->CreateRet(builder->CreateGEP(ctypepointer, { MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head) }));
			}
			return fun;
		}
		bool NomClassType::ArgumentsSubtypes(const TypeList &left, const TypeList &right, bool optimistic)
		{
			size_t size = left.size();
			if (size != right.size())
			{
				throw new std::exception();
			}
			for (size_t i = size; i > 0;)
			{
				i--;
				right[i]->IsSupertype(left[i], optimistic);
				if (!left[i]->IsSupertype(right[i], optimistic))
				{
					return false;
				}
				//i--;
				if (!left[i]->IsSubtype(right[i], optimistic))
				{
					return false;
				}
			}
			return true;
		}

		static const std::string gnprefix = "RT_NOM_CLASSTYPE_";

		NomClassType::NomClassType(const NomNamed *named, const llvm::ArrayRef<NomTypeRef> arguments) : NomType(), GloballyNamed(&gnprefix), id(Count()), head(), Named(named), Arguments(arguments)
		{

		}


		bool NomClassType::IsSubtype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomClassType::IsSubtype(NomBottomTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomClassType::IsSubtype(NomClassTypeRef other, bool optimistic) const
		{
			if (Named == other->Named)
			{
				return ArgumentsSubtypes(Arguments, other->Arguments, optimistic);
			}
			NomSubstitutionContextList nscl(Arguments);
			for (auto super : Named->GetSuperNameds(Arguments))
			{
				if (super->SubstituteSubtyping(&nscl)->IsSubtype(other, optimistic))
				{
					return true;
				}
			}
			return false;
		}
		//bool NomClassType::IsSubtype(NomUnionTypeRef other, bool optimistic) const
		//{
		//	return other->IsSupertype(this);
		//}
		bool NomClassType::IsSubtype(NomTopTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomClassType::IsSubtype(NomTypeVarRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomClassType::IsSupertype(NomTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomClassType::IsSupertype(NomBottomTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomClassType::IsSupertype(NomClassTypeRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}
		bool NomClassType::IsSupertype(NomTopTypeRef other, bool optimistic) const
		{
			return false;
		}
		bool NomClassType::IsSupertype(NomTypeVarRef other, bool optimistic) const
		{
			return other->IsSubtype(this, optimistic);
		}

		NomTypeRef NomClassType::SubstituteSubtyping(const NomSubstitutionContext* context) const
		{
			bool foundChange = false;
			llvm::SmallVector<NomTypeRef, 16> newargs;
			for (auto arg : this->Arguments)
			{
				NomTypeRef sarg = arg->SubstituteSubtyping(context);
				foundChange = foundChange || (sarg != arg);
				newargs.push_back(sarg);
			}
			if (foundChange)
			{
				BoehmVector<NomTypeRef> *allocnewargs = new BoehmVector<NomTypeRef>(this->Arguments.size());
				for (size_t i = newargs.size(); i > 0;)
				{
					i--;
					allocnewargs->operator[](i) = newargs[i];
				}
				return new NomClassType(Named, *allocnewargs);
			}
			return this;
		}

		bool NomClassType::ContainsVariables() const
		{
			for (auto arg : this->Arguments)
			{
				if (arg->ContainsVariables())
				{
					return true;
				}
			}
			return false;
		}

		size_t NomClassType::GetHashCode() const
		{
			NomTypeRefArrayRefHash typesHash;
			return NomClassType::GetHashCode(this->Named, typesHash(this->Arguments));
		}

		size_t NomClassType::GetHashCode(const NomNamed* named, size_t typeArrHash)
		{
			NomStringHash stringHash;
			return stringHash(named->GetName()) + typeArrHash * 31;
		}

		llvm::Type * NomClassType::GetLLVMType() const
		{
			if (Named == NomIntClass::GetInstance())
			{
				return INTTYPE;
			}
			if (Named == NomFloatClass::GetInstance())
			{
				return FLOATTYPE;
			}
			return REFTYPE;
		}

		const std::string NomClassType::GetSymbolRep() const
		{
			std::string ret = "C" + Named->GetSymbolRep() + "$";
			for (auto &arg : Arguments)
			{
				ret += arg->GetSymbolRep();
			}
			return ret + "$$";
		}


		llvm::Constant * NomClassType::createLLVMElement(llvm::Module &mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::StructType * ttt = RTClassType::GetLLVMType(this->Arguments.size());
			llvm::GlobalVariable * gv = new llvm::GlobalVariable(mod, ttt, true, linkage, nullptr, GetGlobalName());

			Function* fun = Function::Create(GetCastFunctionType(), linkage, "MONNOM_RT_TYPECASTFUN_CLS_"+GetGlobalName(), mod);
			{
				fun->setCallingConv(NOMCC);
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
				NomBuilder builder;
				builder->SetInsertPoint(startBlock);
				auto argiter = fun->arg_begin();
				Value* self = argiter;
				argiter++;
				Value* castedValue = argiter;
				
				BasicBlock* failBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Cast Failed!");
				BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "success", fun);
				auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, castedValue);
				auto clsptr = RTClass::GetInterfaceReference(builder, vtable);
				if (!this->Named->IsInterface())
				{
					BasicBlock* directMatchBlock = BasicBlock::Create(LLVMCONTEXT, "directClassMatch", fun);
					BasicBlock* noDirectMatchBlock = BasicBlock::Create(LLVMCONTEXT, "noDirectClassMatch", fun);
					auto directClassMatch = CreatePointerEq(builder, clsptr, this->Named->GetInterfaceDescriptor(mod));
					builder->CreateCondBr(directClassMatch, directMatchBlock, noDirectMatchBlock);

					builder->SetInsertPoint(directMatchBlock);
					if (this->Arguments.size() != 0)
					{
						int argpos = 0;
						for (auto& arg : this->Arguments)
						{
							BasicBlock* nextBlock = BasicBlock::Create(LLVMCONTEXT, "next", fun);
							auto targ = ObjectHeader::GenerateReadTypeArgument(builder, castedValue, argpos);
							RTTypeEq::CreateInlineTypeEqCheck(builder, targ, arg, ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()), ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()), nextBlock, failBlock, failBlock);
							builder->SetInsertPoint(nextBlock);
							argpos++;
						}
					}
					builder->CreateRet(castedValue);

					builder->SetInsertPoint(noDirectMatchBlock);
				}
				BasicBlock* nominalObjectBlock = BasicBlock::Create(LLVMCONTEXT, "nominalObject", fun);
				BasicBlock* structuralObjectBlock = BasicBlock::Create(LLVMCONTEXT, "structuralObject", fun);
				auto isNominalObject = RTVTable::GenerateIsNominalValue(builder, vtable);
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isNominalObject, MakeUInt(1,1) });
				builder->CreateCondBr(isNominalObject, nominalObjectBlock, structuralObjectBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(nominalObjectBlock);
				{
					auto supercount = RTInterface::GenerateReadSuperInstanceCount(builder, clsptr);
					auto superinstances = RTInterface::GenerateReadSuperInstances(builder, clsptr);

				}

				builder->SetInsertPoint(structuralObjectBlock);
				{
					StructuralValueHeader::GenerateMonotonicStructuralCast(builder, fun, successBlock, failBlock, castedValue, this, ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()));
				}
				builder->SetInsertPoint(successBlock);
				builder->CreateRet(castedValue);
			}
			gv->setInitializer(RTClassType::GetConstant(mod, this, fun));
			return llvm::ConstantExpr::getGetElementPtr(gv->getType()->getElementType(), gv, llvm::ArrayRef<llvm::Constant *>({ MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head) }));
		}

		llvm::Value * NomClassType::GenerateRTInstantiation(NomBuilder& builder, CompileEnv* env) const
		{
			if (this->ContainsVariables())
			{
				llvm::Value * argarr[2];
				argarr[0] = this->Named->GetLLVMElement(*(env->Module));
				if (this->Arguments.size() > 0)
				{
					llvm::AllocaInst *typesarr = builder->CreateAlloca(RTTypeHead::GetLLVMType()->getPointerTo(), MakeInt<int32_t>(this->Arguments.size()));
					for (size_t i = 0; i < this->Arguments.size(); i++)
					{
						MakeStore(builder,(*(env->Module)), this->Arguments[i]->GenerateRTInstantiation(builder, env), builder->CreateGEP(typesarr, MakeInt<int32_t>(i)));
					}
					argarr[1] = typesarr;
				}
				
				return builder->CreateCall(env->Module->getFunction("NOM_RTInstantiateClass"), argarr);
			}
			else
			{
				return this->GetLLVMElement(*(env->Module));
			}
		}
		llvm::Constant * NomClassType::findLLVMElement(llvm::Module & mod) const
		{
			llvm::SmallVector<char, 32> buf;
			auto var = mod.getGlobalVariable(GetGlobalName());
			if (var == nullptr)
			{
				return var;
			}
			return llvm::ConstantExpr::getGetElementPtr(var->getType()->getElementType(), var, llvm::ArrayRef<llvm::Constant *>({MakeInt32(0), MakeInt32((unsigned char)RTClassTypeFields::Head)}));
		}
		intptr_t NomClassType::GetRTElement() const
		{
			throw new std::exception();
			//if (head.Entry() != nullptr)
			//{
			//	return reinterpret_cast<intptr_t>(head.Entry());
			//}
			//if (auto var = NomJIT::Instance().lookup(GetGlobalName())->getAddress())
			//{
			//	intptr_t ret = (var) + RTClassType::HeadOffset();
			//	head = RTTypeHead((void *)ret);
			//	return ret;
			//}
			//auto mmod = GetMainModule();
			//if (mmod != nullptr)
			//{
			//	throw new std::exception();
			//}
			//void * rtct = (nmalloc(RTClassType::SizeOf()+sizeof(intptr_t *)*this->Arguments.size()));
			//auto initfun = GetCPPInitializerFunction();
			//return (intptr_t)initfun(rtct, (void*) this->Named->GetRTElement(), this->GetHashCode(), this, this->Arguments.size(), this->Arguments.data());
		}
		NomClassTypeRef NomClassType::GetClassInstantiation(const NomNamed * named) const
		{
			if (named == this->Named)
			{
				return this;
			}
			NomClassTypeRef ret = nullptr;
			for (auto snamed : this->Named->GetSuperNameds(this->Arguments))
			{
				auto super = snamed->GetClassInstantiation(named);
				if (super != nullptr)
				{
					if (ret == nullptr)
					{
						ret = super;
						continue;
					}
					ret = named->GetType(NomClassType::MeetArguments(ret->Arguments, super->Arguments));
				}
			}
			return ret;
		}
		bool NomClassType::IsDisjoint(NomTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomClassType::IsDisjoint(NomBottomTypeRef other) const
		{
			return true;
		}
		bool NomClassType::IsDisjoint(NomClassTypeRef other) const
		{
			if ((!this->Named->IsInterface()) || (!other->Named->IsInterface()))
			{
				return !(this->IsSubtype(other, true) || other->IsSubtype(this, true));
			}
			return false;
		}
		bool NomClassType::IsDisjoint(NomTopTypeRef other) const
		{
			return false;
		}
		bool NomClassType::IsDisjoint(NomTypeVarRef other) const
		{
			return other->IsDisjoint(this);
		}
		bool NomClassType::IsSubtype(NomDynamicTypeRef other, bool optimistic) const
		{
			return true;
		}
		bool NomClassType::IsSupertype(NomDynamicTypeRef other, bool optimistic) const
		{
			return optimistic;
		}
		bool NomClassType::PossiblyPrimitive() const
		{
			return NomIntClass::GetInstance()->GetType()->IsSubtype(this) || NomFloatClass::GetInstance()->GetType()->IsSubtype(this) || NomIntClass::GetInstance()->GetType()->GetClassInstantiation(this->Named) != nullptr || NomFloatClass::GetInstance()->GetType()->GetClassInstantiation(this->Named) != nullptr;
		}
		bool NomClassType::UncertainlyPrimitive() const
		{
			auto inttype = NomIntClass::GetInstance()->GetType();
			auto floattype = NomFloatClass::GetInstance()->GetType();
			return !((IsDisjoint(inttype) && IsDisjoint(floattype)) || IsSubtype(inttype, false) || IsSubtype(floattype, false));
		}
		bool NomClassType::IsSubtype(NomMaybeTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this, optimistic);
		}
		bool NomClassType::IsSupertype(NomMaybeTypeRef other, bool optimistic) const
		{
			return other->IsSupertype(this,optimistic);
		}
		bool NomClassType::IsDisjoint(NomMaybeTypeRef other) const
		{
			return other->IsDisjoint(this);
		}
		TypeReferenceType NomClassType::GetTypeReferenceType() const
		{
			if (Named == NomIntClass::GetInstance())
			{
				return TypeReferenceType::UnpackedInteger;
			}
			if (Named == NomFloatClass::GetInstance())
			{
				return TypeReferenceType::UnpackedFloat;
			}
			//if (Named == NomBoolClass::GetInstance()) //Dangerous for interaction with c++ code, need to adjust at least the signatures of the internal wrapper methods
			//{
			//	return TypeReferenceType::UnpackedBool;
			//}
			return TypeReferenceType::Reference; //llvm::StructType::create(LLVMCONTEXT)->getPointerTo();
		}
		bool NomClassType::ContainsVariableIndex(int index) const
		{
			for (auto& arg : Arguments)
			{
				if (arg->ContainsVariableIndex(index))
				{
					return true;
				}
			}
			return false;
		}
	}
}

using namespace Nom::Runtime;

extern "C" intptr_t NOM_RTInstantiateClass(intptr_t cls, intptr_t *args)
{
	NomClass *ncls = reinterpret_cast<NomClass *>(cls);
	NomTypeRef *argtypes = reinterpret_cast<NomTypeRef *>(args);
	return ncls->GetType(TypeList(argtypes, ncls->GetTypeParametersCount()))->GetRTElement();
}