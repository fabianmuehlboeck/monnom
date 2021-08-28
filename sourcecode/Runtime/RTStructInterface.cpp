#include "RTStructInterface.h"
#include "RTInterface.h"
#include "RTGeneralInterface.h"
#include "CompileHelpers.h"
#include "CastStats.h"
#include "RTConfig.h"
#include "NomVMInterface.h"
#include "RTVTable.h"
#include "RTInterfaceTableEntry.h"
#include "NomInterface.h"
#include "NomMethod.h"
#include "NomMethodTableEntry.h"
#include "NomCallableVersion.h"
#include "llvm/ADT/SmallVector.h"
#include "StructHeader.h"
#include "NomNameRepository.h"
#include "instructions/CallDispatchBestMethod.h"
#include "RTCast.h"
#include "CompileEnv.h"
#include "NomDynamicType.h"
#include "RefValueHeader.h"
#include "NomType.h"
#include "RTOutput.h"
#include "NomPartialApplication.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "NomTypeParameter.h"
#include "CallingConvConf.h"
#include "IMT.h"
#include "NomStruct.h"
#include "NomMethodKey.h"
#include "RTSignature.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		class StructCheckCompileEnv : public ACompileEnv
		{
		public:
			StructCheckCompileEnv(llvm::Function* function, llvm::ArrayRef<NomTypeParameterRef> typeParams, const NomMemberContext* context, TypeList argTypes) :ACompileEnv(argTypes.size() + 1, "", function, nullptr, typeParams, context, argTypes, &NomDynamicType::Instance())
			{

			}
			// Inherited via CompileEnv
			virtual NomTypeVarValue GetTypeArgument(NomBuilder& builder, int i) override
			{
				if (((size_t)i) < Context->GetTypeParametersCount())
				{
					return NomTypeVarValue(MakeLoad(builder, builder->CreateGEP(RTStructInterface::GetTypeArgumentsPointer(builder, RefValueHeader::GenerateReadVTablePointer(builder, registers[0])), MakeInt32(-(i + 1))), "typeArg"), Context->GetTypeParameter(i)->GetVariable());
				}
				return TypeArguments[i - Context->GetTypeParametersCount()];
			}
			virtual size_t GetEnvTypeArgumentCount() override
			{
				return Context->GetTypeParametersCount();
			}
			virtual llvm::Value* GetEnvTypeArgumentArray(NomBuilder& builder) override
			{
				return RTStructInterface::GetTypeArgumentsPointer(builder, RefValueHeader::GenerateReadVTablePointer(builder, registers[0]));
			}
		};

		llvm::StructType* RTStructInterface::GetLLVMType()
		{
			static auto sit = StructType::create(LLVMCONTEXT, "RT_NOM_StructInterface");
			static bool once = true;
			if (once)
			{
				once = false;
				sit->setBody(
					arrtype(TYPETYPE, 0),						//typeargs
					RTInterface::GetLLVMType(),		//regular interface stuff
					RTGeneralInterface::GetLLVMPointerType()	//pointer to original
				);
			}
			return sit;
		}

		llvm::Value* RTStructInterface::GetTypeArgumentsPointer(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTStructInterfaceFields::TypeArgs), MakeInt32(0) });
		}

		llvm::Value* RTStructInterface::GetTypeArgumentNegativeIndex(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* index)
		{
			return MakeLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(RTStructInterfaceFields::TypeArgs), index });
		}

		llvm::Value* RTStructInterface::GetOriginalInterface(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeLoad(builder, vtablePtr, GetLLVMPointerType(), MakeInt32(RTStructInterfaceFields::OrigInterface), "originalInterface");
		}

		llvm::Constant* RTStructInterface::CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* instantiationDictionary, llvm::Constant* origInterfacePointer, bool optimizedTypeVarAccess)
		{
			NomBuilder builder;

			Function* imtFun = nullptr;

			Constant** imtArr = makealloca(Constant*, IMTsize);

			for (decltype(IMTsize) i = 0; i < IMTsize; i++)
			{
				imtArr[i] = ConstantPointerNull::get(GetIMTFunctionType()->getPointerTo());
				//NomMethodKey* nmk = nullptr;
				//for (auto mte : irptr->MethodTable)
				//{
				//	if (mte->Method->GetIMTIndex() == i)
				//	{
				//		auto nnmk = NomMethodKey::GetMethodKey(mte->Method);
				//		if (nmk == nullptr || nmk == nnmk)
				//		{
				//			nmk = nnmk;
				//		}
				//		else
				//		{
				//			nmk = nullptr;
				//			break;
				//		}
				//	}
				//}
				//if (nmk == nullptr)
				//{
				//	if (imtFun == nullptr)
				//	{
				//		imtFun = Function::Create(GetIMTFunctionType(), linkage, "", mod);
				//		imtFun->setCallingConv(NOMCC);
				//		BasicBlock* imtBB = BasicBlock::Create(LLVMCONTEXT, "", imtFun);

				//		builder->SetInsertPoint(imtBB);

				//		Value* args[5];
				//		assert(imtFun->arg_size() == 5);
				//		int argpos = 0;
				//		for (auto& arg : imtFun->args())
				//		{
				//			args[argpos] = &arg;
				//			argpos++;
				//		}

				//		Value* callFun = args[0];
				//		Value* receiver = args[1];
				//		llvm::Value* instanceTypeArr = ConstantPointerNull::get(TYPETYPE->getPointerTo());
				//		if (optimizedTypeVarAccess)
				//		{
				//			instanceTypeArr = StructHeader::GeneratePointerToCastTypeArguments(builder, receiver);
				//		}
				//		else if(irptr->GetTypeParametersCount() > 0)
				//		{
				//			auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				//			instanceTypeArr = RTStructInterface::GetTypeArgumentsPointer(builder, vtable);
				//		}
				//		args[0] = instanceTypeArr;
				//		auto castFunCall = builder->CreateCall(GetIMTCastFunctionType(), callFun, ArrayRef<Value*>(args, 5));
				//		castFunCall->setCallingConv(NOMCC);

				//		builder->CreateRet(castFunCall);
				//	}
				//	imtArr[i] = imtFun;
				//}
				//else
				//{
				//	Function* specializedImtFun = Function::Create(GetIMTFunctionType(), linkage, "", mod);
				//	specializedImtFun->setCallingConv(NOMCC);
				//	BasicBlock* imtBB = BasicBlock::Create(LLVMCONTEXT, "", specializedImtFun);

				//	builder->SetInsertPoint(imtBB);

				//	Value* args[5];
				//	assert(specializedImtFun->arg_size() == 5);
				//	int argpos = 0;
				//	for (auto& arg : specializedImtFun->args())
				//	{
				//		args[argpos] = &arg;
				//		argpos++;
				//	}
				//	Value* receiver = args[1];
				//	llvm::Value* instanceTypeArr = ConstantPointerNull::get(TYPETYPE->getPointerTo());
				//	if (optimizedTypeVarAccess)
				//	{
				//		instanceTypeArr = StructHeader::GeneratePointerToCastTypeArguments(builder, receiver);
				//	}
				//	else if (irptr->GetTypeParametersCount() > 0)
				//	{
				//		auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				//		instanceTypeArr = RTStructInterface::GetTypeArgumentsPointer(builder, vtable);
				//	}

				//	args[0] = instanceTypeArr;

				//	auto castFunCall = builder->CreateCall(nmk->GetLLVMElement(mod), ArrayRef<Value*>(args, 5));
				//	castFunCall->setCallingConv(NOMCC);

				//	builder->CreateRet(castFunCall);

				//	imtArr[i] = specializedImtFun;
				//}
			}

			Constant* imt = ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), llvm::ArrayRef<Constant*>(imtArr, IMTsize));

			StructType* gvartype = StructType::get(LLVMCONTEXT, {/* irptr->GetInterfaceTableType(false),*/ GetLLVMType() }, true);
			GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, name);

			gvar->setInitializer(ConstantStruct::get(gvartype, { /*irptr->GetInterfaceTable(mod, linkage),*/ /*ConstantArray::get(arrtype(POINTERTYPE, mtsize), ArrayRef<Constant*>(funbuf, mtsize)),*/ ConstantStruct::get(GetLLVMType(), {ConstantArray::get(arrtype(TYPETYPE,0),{}), RTInterface::CreateConstant((optimizedTypeVarAccess ? RTDescriptorKind::OptimizedStruct : RTDescriptorKind::Struct), irptr, RTInterfaceFlags::IsInterface , typeArgCount, superTypesCount, superTypeEntries, imt, ConstantPointerNull::get(GetCheckReturnValueFunctionType()->getPointerTo()), ConstantPointerNull::get(GetMethodEnsureFunctionType()->getPointerTo()), instantiationDictionary, ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo())),origInterfacePointer}) }));
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(/*1*/ 0) }));
		}
		llvm::Value* RTStructInterface::CreateCopyVTable(NomBuilder& builder, llvm::Value* vtable/*, llvm::Value* typeArgCount*/)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncVTableAlloactionsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			auto stemplate = RTGeneralInterface::GenerateReadTemplate(builder, vtable);
			auto copybytes = RTGeneralInterface::GenerateReadTemplateSize(builder, vtable);
			auto templateOffset = RTGeneralInterface::GenerateReadTemplateOffset(builder, vtable);
			auto typeArgCount = RTInterface::GenerateReadTypeArgCount(builder, vtable);
			auto allocsize = builder->CreateAdd(builder->CreateAdd(builder->CreateURem(builder->CreateSub(MakeInt<size_t>(8), builder->CreateURem(copybytes, MakeInt<size_t>(8))), MakeInt<size_t>(8)), copybytes), builder->CreateMul(llvmsizeof(TYPETYPE), EnsureIntegerSize(builder, typeArgCount, 64)));

			auto allocfun = GetAlloc(builder->GetInsertBlock()->getParent()->getParent());
			auto newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { allocsize }, "newvtable"), TYPETYPE);

			newmem = builder->CreateGEP(newmem, typeArgCount);
			newmem = builder->CreatePointerCast(newmem, POINTERTYPE);

			builder->CreateMemCpy(newmem, llvm::MaybeAlign(8), builder->CreateGEP(builder->CreatePointerCast(stemplate, POINTERTYPE), builder->CreateNeg(EnsureIntegerSize(builder, templateOffset, 32))), llvm::MaybeAlign(8), EnsureIntegerSize(builder, copybytes, 32));

			//auto copybytes = builder->CreatePtrDiff(builder->CreatePointerCast(builder->CreateGEP(builder->CreateGEP(vtable, { MakeInt32(0), MakeInt32(RTStructInterfaceFields::OrigInterface) }), MakeInt32(1)), POINTERTYPE), builder->CreatePointerCast(vtable, POINTERTYPE));
			//auto prefixSize = builder->CreatePtrDiff(builder->CreatePointerCast(vtable, POINTERTYPE), builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(vtable, RTVTable::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), MakeInt32(-2) }), POINTERTYPE));
			//auto sizeinbytes = builder->CreatePtrDiff(builder->CreatePointerCast(builder->CreateGEP(ConstantPointerNull::get(GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTStructInterfaceFields::TypeArgs), typeArgCount }), POINTERTYPE), builder->CreatePointerCast(builder->CreateGEP(ConstantPointerNull::get(RTVTable::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), MakeInt32(-2) }), POINTERTYPE));


			auto newmemasvtable = builder->CreatePointerCast(builder->CreateGEP(newmem, EnsureIntegerSize(builder, templateOffset, 32)), RTStructInterface::GetLLVMType()->getPointerTo());
			return newmemasvtable;
		}
	}
}