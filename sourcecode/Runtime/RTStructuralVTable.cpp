#include "RTStructuralVTable.h"
#include "RTInterface.h"
#include "RTVTable.h"
#include "RTCompileConfig.h"
#include "IMT.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "NomInterface.h"
#include "NomBuilder.h"
#include "StructHeader.h"
#include "LambdaHeader.h"
#include "RTOutput.h"
#include "CallingConvConf.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"
#include "NomNameRepository.h"
#include "RTSignature.h"
#include "StructuralValueHeader.h"
#include "RTSTable.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		RTStructuralVTable::RTStructuralVTable()
		{
		}
		RTStructuralVTable& RTStructuralVTable::Instance()
		{
			static RTStructuralVTable rtsvt; return rtsvt;
		}
		StructType* RTStructuralVTable::GetLLVMType()
		{
			return RTInterface::GetLLVMType();
		}
		Constant* RTStructuralVTable::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			GlobalVariable* gvar = mod.getNamedGlobal("MONNOM_RT_STRUCTURALVTABLE");
			if (gvar == nullptr)
			{
				gvar = new GlobalVariable(mod, GetLLVMType(), true, linkage, nullptr, "MONNOM_RT_STRUCTURALVTABLE");
				Constant** imt = makealloca(Constant*, IMTsize);
				Function* imtfun = Function::Create(GetIMTFunctionType(), linkage, "MONNOM_RT_STRUCTURALIMTFUN", mod);
				imtfun->setCallingConv(NOMCC);
				NomBuilder builder;
				BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", imtfun);
				builder->SetInsertPoint(startBlock);
				//BasicBlock* returnBlock = BasicBlock::Create(LLVMCONTEXT, "start", imtfun);
				//static const char* castFailMsg = "Failing cast!";
				//BasicBlock* castErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, castFailMsg);
				//static const char* errorMsg = "Invalid structural value tag";
				//BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, errorMsg);
				static const char* unimplementedMsg = "Unimplemented!";
				BasicBlock* unimplementedBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplementedMsg);

				auto fargs = imtfun->arg_begin();

				Value* handlerFunPtr = builder->CreatePointerCast(fargs, GetIMTCastFunctionType()->getPointerTo());

				Value** args = makealloca(Value*, 2 + RTConfig_NumberOfVarargsArguments);
				for (int i = 0; i < 2+RTConfig_NumberOfVarargsArguments; i++)
				{
					args[i] = fargs;
					fargs++;
				}

				auto callReturnValue = builder->CreateCall(GetIMTCastFunctionType(), handlerFunPtr, ArrayRef<Value*>(args, 2 + RTConfig_NumberOfVarargsArguments));
				callReturnValue->setCallingConv(NOMCC);
				callReturnValue->setTailCallKind(CallInst::TailCallKind::TCK_MustTail);
				//Value* receiver = args[0];

				////BasicBlock* lambdaBlock = nullptr, *recordBlock=nullptr;
				//Value* sTable = StructuralValueHeader::GenerateReadSTablePtr(builder, receiver);
				//{
				//	BasicBlock* castListBlock = unimplementedBlock;//BasicBlock::Create(LLVMCONTEXT, "lambdaCastList", imtfun);
				//	BasicBlock* singleCastBlock = BasicBlock::Create(LLVMCONTEXT, "lambdaSingleCast", imtfun);
				//	Value* castTypePtr = StructuralValueHeader::GenerateReadCastData(builder, receiver);
				//	Value* castTypeTag = builder->CreateTrunc(builder->CreatePtrToInt(castTypePtr, numtype(intptr_t)), inttype(1));
				//	builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { castTypeTag, MakeUInt(1, 0) });
				//	builder->CreateCondBr(castTypeTag, castListBlock, singleCastBlock);

				//	/*builder->SetInsertPoint(castListBlock);
				//	{

				//	}*/

				//	builder->SetInsertPoint(singleCastBlock);
				//	{
				//		Value* iface = RTClassType::GenerateReadClassDescriptorLink(builder, castTypePtr);
				//		Value* typeargs = RTClassType::GetTypeArgumentsPtr(builder, castTypePtr);
				//		Value* checkFun = RTVTable::GenerateReadReturnValueCheckFunction(builder, iface);
				//		auto checkFunResult = builder->CreateCall(GetCheckReturnValueFunctionType(), checkFun, { handlerFunPtr, typeargs, callReturnValue, args[1], args[2], args[3] });
				//		checkFunResult->setCallingConv(NOMCC);
				//		builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { checkFunResult, MakeUInt(1, 1) });
				//		builder->CreateCondBr(checkFunResult, returnBlock, castErrorBlock);
				//	}
				//}


				//StructuralValueHeader::GenerateStructuralValueKindCase(builder, receiver, &sTable, &lambdaBlock, &recordBlock, &unimplementedBlock, &unimplementedBlock);

				//Value* sTablePtr = StructHeader::GenerateReadStructDescriptorPtr(builder, receiver);
				//Value* sTableTag = builder->CreateTrunc(builder->CreatePtrToInt(sTablePtr, INTTYPE), inttype(3));
				//auto sTableTagSwitch = builder->CreateSwitch(sTableTag, errorBlock, 2);

				//BasicBlock* lambdaBlock = BasicBlock::Create(LLVMCONTEXT, "castResultsForLambda", imtfun);
				//BasicBlock* structBlock = BasicBlock::Create(LLVMCONTEXT, "castResultsForRecord", imtfun);

				//sTableTagSwitch->addCase(MakeInt(3, (uint64_t)RTDescriptorKind::Lambda), lambdaBlock);
				//sTableTagSwitch->addCase(MakeInt(3, (uint64_t)RTDescriptorKind::Struct), structBlock);

				//builder->SetInsertPoint(recordBlock);
				//{

				//}
				//builder->CreateBr(unimplementedBlock);

				//builder->SetInsertPoint(lambdaBlock);
				//{
				//	BasicBlock* castListBlock = unimplementedBlock;//BasicBlock::Create(LLVMCONTEXT, "lambdaCastList", imtfun);
				//	BasicBlock* singleCastBlock = BasicBlock::Create(LLVMCONTEXT, "lambdaSingleCast", imtfun);
				//	Value* castTypePtr = LambdaHeader::GenerateReadCastTypePointer(builder, receiver);
				//	Value* castTypeTag = builder->CreateTrunc(builder->CreatePtrToInt(castTypePtr, numtype(intptr_t)), inttype(1));
				//	builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { castTypeTag, MakeUInt(1, 0) });
				//	builder->CreateCondBr(castTypeTag, castListBlock, singleCastBlock);

				//	/*builder->SetInsertPoint(castListBlock);
				//	{

				//	}*/

				//	builder->SetInsertPoint(singleCastBlock);
				//	{
				//		Value* iface = RTClassType::GenerateReadClassDescriptorLink(builder, castTypePtr);
				//		Value* typeargs = RTClassType::GetTypeArgumentsPtr(builder, castTypePtr);
				//		Value* checkFun = RTVTable::GenerateReadReturnValueCheckFunction(builder, iface);
				//		auto checkFunResult = builder->CreateCall(GetCheckReturnValueFunctionType(), checkFun, { handlerFunPtr, typeargs, callReturnValue, args[1], args[2], args[3] });
				//		checkFunResult->setCallingConv(NOMCC);
				//		builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { checkFunResult, MakeUInt(1, 1) });
				//		builder->CreateCondBr(checkFunResult, returnBlock, castErrorBlock);
				//	}
				//}
				//builder->SetInsertPoint(returnBlock);
				builder->CreateRet(callReturnValue);

				llvm::raw_os_ostream out(std::cout);

				if (verifyFunction(*imtfun, &out))
				{
					std::cout << "Could not verify structural vtable IMT method!";
					imtfun->print(out);
					out.flush();
					std::cout.flush();
					throw "MONNOM_RT_STRUCTURALVTABLE";
				}

				for (decltype(IMTsize) i = 0; i < IMTsize; i++)
				{
					imt[i] = imtfun;
				}

				llvm::Function* methodEnsureFunction = Function::Create(GetMethodEnsureFunctionType(), linkage, "MONNOM_RT_STRUCTURALVTABLE_ENSUREMETHOD", mod);
				methodEnsureFunction->setCallingConv(NOMCC);
				{
					BasicBlock* mEStartBlock = BasicBlock::Create(LLVMCONTEXT, "start", methodEnsureFunction);
					BasicBlock* mEStructBlock = BasicBlock::Create(LLVMCONTEXT, "ensureMethodInRecord", methodEnsureFunction);
					BasicBlock* mENonStructBlock = BasicBlock::Create(LLVMCONTEXT, "notARecord", methodEnsureFunction);
					auto args = methodEnsureFunction->arg_begin();
					Value* receiver = args;
					args++;
					Value* methodName = args;
					builder->SetInsertPoint(mEStartBlock);
					auto sTablePtr = StructHeader::GenerateReadStructDescriptorPtr(builder, receiver);
					auto sTableTag = builder->CreateTrunc(builder->CreatePtrToInt(sTablePtr, numtype(intptr_t)), inttype(3));
					builder->CreateCondBr(builder->CreateICmpEQ(sTableTag, MakeUInt(3, (uint64_t)RTDescriptorKind::Struct)), mEStructBlock, mENonStructBlock);

					builder->SetInsertPoint(mEStructBlock);
					auto structMethodEnsureFunction = RTStruct::GenerateReadMethodEnsure(builder, builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(sTablePtr, numtype(intptr_t)), ConstantExpr::getXor(ConstantInt::getAllOnesValue(INTTYPE), MakeInt<intptr_t>(7))), POINTERTYPE));
					auto callStructMethodEnsure = builder->CreateCall(GetMethodEnsureFunctionType(), structMethodEnsureFunction, { receiver, methodName });
					callStructMethodEnsure->setCallingConv(NOMCC);
					builder->CreateRet(callStructMethodEnsure);

					builder->SetInsertPoint(mENonStructBlock);
					builder->CreateRet(builder->CreateICmpEQ(methodName, MakeInt<size_t>(NomNameRepository::Instance().GetNameID(""))));

					if (verifyFunction(*methodEnsureFunction, &out))
					{
						std::cout << "Could not verify structural vtable methodEnsure function!";
						methodEnsureFunction->print(out);
						out.flush();
						std::cout.flush();
						throw "MONNOM_RT_STRUCTURALVTABLE_ENSUREMETHOD";
					}
				}

				gvar->setInitializer(RTInterface::CreateConstant(nullptr, RTInterfaceFlags::IsInterface, MakeInt32(0), MakeInt<size_t>(0), ConstantPointerNull::get(SuperInstanceEntryType()->getPointerTo()), ConstantArray::get(arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize), ArrayRef<Constant*>(imt, IMTsize)), ConstantPointerNull::get(GetCheckReturnValueFunctionType()->getPointerTo()), methodEnsureFunction, ConstantPointerNull::get(POINTERTYPE), ConstantPointerNull::get(RTSignature::GetLLVMType()->getPointerTo())));
			}
			return gvar;
		}
		Constant* RTStructuralVTable::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getNamedGlobal("MONNOM_RT_STRUCTURALVTABLE");
		}
	}
}