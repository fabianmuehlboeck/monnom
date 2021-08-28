#include "RTLambdaInterface.h"
#include "RTInterface.h"
#include "RTFunctionalInterface.h"
#include "NomInterface.h"
#include "NomPartialApplication.h"
#include "CompileHelpers.h"
#include "RTInterfaceTableEntry.h"
#include "RTVTable.h"
#include "RTConfig.h"
#include "CastStats.h"
#include "IMT.h"
#include "NomMethodTableEntry.h"
#include "NomMethodKey.h"
#include "CallingConvConf.h"
#include "LambdaHeader.h"
#include "RefValueHeader.h"
#include "RTSignature.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTLambdaInterface::GetLLVMType()
		{
			static llvm::StructType* rtlit = llvm::StructType::create(LLVMCONTEXT, "NOM_RT_LambdaInterface");
			static bool once = true;
			if (once)
			{
				once = false;
				rtlit->setBody({
					arrtype(TYPETYPE, 0), //type arguments
					RTInterface::GetLLVMType(),			//regular interface representation
					RTFunctionalInterface::GetLLVMType()->getPointerTo(), //parent
					POINTERTYPE //pointer to wrapper function for raw invoke; null if raw function can be used directly
					});
			}
			return rtlit;
		}

		void RTLambdaInterface::GenerateWriteMethodPointer(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* methodPtr, llvm::Value* index)
		{
			MakeStore(builder, methodPtr, vtablePtr, RTVTable::GetLLVMType()->getPointerTo(), { MakeInt32(RTVTableFields::MethodTable), builder->CreateSub(MakeInt32(-1), EnsureIntegerSize(builder, index, 32)) });
		}
		llvm::Value* RTLambdaInterface::GetTypeArgumentsPointer(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::TypeArgs), MakeInt32(0) });
		}

		llvm::Value* RTLambdaInterface::GetTypeArgumentNegativeIndex(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* index)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::TypeArgs), index }));
		}

		llvm::Constant* RTLambdaInterface::CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, RTDescriptorKind kind, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* origInterfacePointer, llvm::Constant* instantiationDictionary, llvm::Function* functionptr, bool optimizedTypeVarAccess)
		{
			StructType* gvartype = StructType::get(LLVMCONTEXT, { /*arrtype(RTInterfaceTableEntry::GetLLVMType(), 1),*/ /*arrtype(POINTERTYPE, 1),*/ GetLLVMType() }, true);
			GlobalVariable* gvar = new GlobalVariable(mod, gvartype, true, linkage, nullptr, name);
			
			Function* imtFun = nullptr;

			NomBuilder builder;

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
				//			instanceTypeArr = LambdaHeader::GeneratePointerToCastTypeArguments(builder, receiver);
				//		}
				//		else if (irptr->GetTypeParametersCount() > 0)
				//		{
				//			auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				//			instanceTypeArr = RTLambdaInterface::GetTypeArgumentsPointer(builder, vtable);
				//		}
				//		args[0] = instanceTypeArr;
				//		auto castFunCall = builder->CreateCall(GetIMTCastFunctionType(), callFun, ArrayRef<Value*>(args , 5));
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
				//		instanceTypeArr = LambdaHeader::GeneratePointerToCastTypeArguments(builder, receiver);
				//	}
				//	else if (irptr->GetTypeParametersCount() > 0)
				//	{
				//		auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				//		instanceTypeArr = RTLambdaInterface::GetTypeArgumentsPointer(builder, vtable);
				//	}
				//	args[0] = instanceTypeArr;

				//	auto castFunCall = builder->CreateCall(nmk->GetLLVMElement(mod), ArrayRef<Value*>(args, 5));
				//	castFunCall->setCallingConv(NOMCC);

				//	builder->CreateRet(castFunCall);

				//	imtArr[i] = specializedImtFun;
				//}
			}

			Constant* imt = ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), llvm::ArrayRef<Constant*>(imtArr, IMTsize));

			llvm::Function* wrap = GenerateRawInvokeWrap(&mod, linkage, "", irptr, functionptr);

			gvar->setInitializer(ConstantStruct::get(gvartype, { /*ConstantArray::get(arrtype(InterfaceTableEntryType(), 1), {RTInterfaceTableEntry::CreateConstant(irptr->GetID(), 0)}),*//* ConstantArray::get(arrtype(POINTERTYPE, 1), {ConstantExpr::getPointerCast(functionptr, POINTERTYPE)}),*/ ConstantStruct::get(GetLLVMType(), {ConstantArray::get(arrtype(TYPETYPE,0),{}), RTInterface::CreateConstant(kind, irptr, RTInterfaceFlags::IsInterface | RTInterfaceFlags::IsFunctional, typeArgCount, superTypesCount, superTypeEntries, imt, ConstantPointerNull::get(GetCheckReturnValueFunctionType()->getPointerTo()), ConstantPointerNull::get(GetMethodEnsureFunctionType()->getPointerTo()), instantiationDictionary, ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo())),origInterfacePointer , ConstantExpr::getPointerCast(wrap, POINTERTYPE)}) }));
			return ConstantExpr::getGetElementPtr(gvartype, gvar, ArrayRef<Constant*>({ MakeInt32(0), MakeInt32(/*2*/ /*1*/ 0) }));
		}
		llvm::Value* RTLambdaInterface::CreateCopyVTable(NomBuilder& builder, llvm::Value* vtable, llvm::Value* typeArgCount)
		{
			if (NomCastStats)
			{
				builder->CreateCall(GetIncVTableAlloactionsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			//this assumes that there are no method table entries and no interface table entries
			auto allocfun = GetAlloc(builder->GetInsertBlock()->getParent()->getParent());
			auto copybytes = MakeInt(32, builder->GetInsertBlock()->getParent()->getParent()->getDataLayout().getTypeAllocSize(GetLLVMType()).getKnownMinSize());
			//builder->CreatePtrToInt(builder->CreateGEP(ConstantPointerNull::get(GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::TypeArgs) }), inttype(32));
			auto sizeOfRawElem = ConstantExpr::getPtrToInt(ConstantExpr::getGetElementPtr(GetLLVMType(), ConstantPointerNull::get(GetLLVMType()->getPointerTo()), MakeInt32(1)), numtype(size_t));
			auto sizeOfTypeArr = builder->CreatePtrToInt(builder->CreateGEP(ConstantPointerNull::get(TYPETYPE), typeArgCount), numtype(size_t));
			auto sizeinbytes = builder->CreateAdd(sizeOfRawElem, sizeOfTypeArr); //builder->CreatePtrDiff(builder->CreatePointerCast(builder->CreateGEP(ConstantPointerNull::get(GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::TypeArgs), typeArgCount }), POINTERTYPE), ConstantPointerNull::get(POINTERTYPE));
			auto newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { sizeinbytes }, "newvtable"), TYPETYPE);
			newmem = builder->CreateGEP(newmem, typeArgCount);
			newmem = builder->CreatePointerCast(newmem, POINTERTYPE);
			builder->CreateMemCpyInline(newmem, llvm::MaybeAlign(4), builder->CreatePointerCast(vtable, POINTERTYPE), llvm::MaybeAlign(4), copybytes);
			return builder->CreatePointerCast(newmem, RTLambdaInterface::GetLLVMType()->getPointerTo());

			////this assumes there is exactly one method table entry and one interface table entry
			//auto copybytes = builder->CreatePtrDiff(builder->CreatePointerCast(builder->CreateGEP(builder->CreateGEP(vtable, { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::OrigInterface) }), MakeInt32(1)), POINTERTYPE), builder->CreatePointerCast(vtable, POINTERTYPE));
			//auto prefixSize = builder->CreatePtrDiff(builder->CreatePointerCast(vtable, POINTERTYPE), builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(vtable, RTVTable::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), MakeInt32(-2) }), POINTERTYPE));
			//auto sizeinbytes = builder->CreatePtrDiff(builder->CreatePointerCast(builder->CreateGEP(ConstantPointerNull::get(GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaInterfaceFields::TypeArgs), typeArgCount }), POINTERTYPE), builder->CreatePointerCast(builder->CreateGEP(ConstantPointerNull::get(RTVTable::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), MakeInt32(-2) }), POINTERTYPE));
			//auto allocfun = GetAlloc(builder->GetInsertBlock()->getParent()->getParent());
			//auto newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { sizeinbytes }, "newvtable"), POINTERTYPE);
			//builder->CreateMemCpy(newmem, llvm::MaybeAlign(4), builder->CreatePointerCast(builder->CreateGEP(builder->CreatePointerCast(vtable, RTVTable::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceTable), MakeInt32(-2) }), POINTERTYPE), llvm::MaybeAlign(4), EnsureIntegerSize(builder, copybytes, 32));
			//auto newmemasvtable = builder->CreatePointerCast(builder->CreateGEP(newmem, EnsureIntegerSize(builder, prefixSize, 32)), RTLambdaInterface::GetLLVMType()->getPointerTo());
			//return newmemasvtable;
		}
		llvm::Value* RTLambdaInterface::GetRawInvokeWrapper(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32((unsigned char)RTLambdaInterfaceFields::RawInvokeWrapper) }));
		}
		llvm::Value* RTLambdaInterface::GenerateReadOrigInterface(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32((unsigned char)RTLambdaInterfaceFields::OrigInterface) }));
		}
	}
}