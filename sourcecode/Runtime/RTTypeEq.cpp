#include "RTTypeEq.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "RTTypeHead.h"
#include "RTClassType.h"
#include "RTTypeVar.h"
#include "RTOutput.h"
#include "RTInstanceType.h"
#include "CallingConvConf.h"
#include "RTMaybeType.h"
#include "RTInterface.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		RTTypeEq::RTTypeEq()
		{

		}
		RTTypeEq& RTTypeEq::Instance()
		{
			static RTTypeEq rtte; return rtte;
		}
		//Assumptions:
		//right type contains no type variables (recursions up to the first type variable were inlined before, and type variable was substituted with something that couldn't contain more variables
		//left type contains no dynamic type (always taken from fully concrete type)
		//left type args contain no more type args and no dynamic type
		llvm::FunctionType* RTTypeEq::GetLLVMFunctionType()
		{
			static FunctionType* ft = FunctionType::get(inttype(1), { TYPETYPE, TYPETYPE->getPointerTo(), TYPETYPE }, false);
			return ft;
		}
		llvm::Function* RTTypeEq::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			FunctionType* funtype = GetLLVMFunctionType();
			Function* fun = Function::Create(funtype, linkage, "RT_NOM_TypeEQ", &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;
			llvm::Argument* arg = fun->arg_begin();
			llvm::Value* lefttype = arg;
			arg++;
			llvm::Value* leftsubsts = arg;
			arg++;
			llvm::Value* righttype = arg;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "typeEqSuccess", fun);
			BasicBlock* failBlock = BasicBlock::Create(LLVMCONTEXT, "typeEqFail", fun);
			BasicBlock* leftRecurseBlock = BasicBlock::Create(LLVMCONTEXT, "leftRecurse", fun);
			builder->SetInsertPoint(startBlock);
			builder->CreateCondBr(CreatePointerEq(builder, lefttype, righttype, "typePtrEq"), successBlock, leftRecurseBlock);

			builder->SetInsertPoint(successBlock);
			builder->CreateRet(MakeUInt(1, 1));

			builder->SetInsertPoint(failBlock);
			builder->CreateRet(MakeUInt(1, 0));

			builder->SetInsertPoint(leftRecurseBlock);
			auto leftPHI = builder->CreatePHI(lefttype->getType(), 2, "leftType");
			leftPHI->addIncoming(lefttype, startBlock);
			BasicBlock* leftClassTypeBlock = nullptr, * leftTopTypeBlock = nullptr, * leftTypeVarBlock = nullptr, * leftBottomTypeBlock = nullptr, * leftInstanceTypeBlock = nullptr, *leftDynamicTypeBlock = nullptr, * leftMaybeTypeBlock = nullptr;

			RTTypeHead::GenerateTypeKindSwitch(builder, leftPHI, &leftClassTypeBlock, &leftTopTypeBlock, &leftTypeVarBlock, &leftBottomTypeBlock, &leftInstanceTypeBlock, &failBlock, &failBlock, &leftDynamicTypeBlock, &leftMaybeTypeBlock, failBlock);

			if (leftTypeVarBlock != nullptr) //this should only happen once
			{
				builder->SetInsertPoint(leftTypeVarBlock);
				auto leftIndex = RTTypeVar::GenerateLoadIndex(builder, leftPHI);
				auto newLeft = MakeLoad(builder, builder->CreateGEP(leftsubsts, builder->CreateSub(MakeIntLike(leftIndex, -1), leftIndex)));
				leftPHI->addIncoming(newLeft, builder->GetInsertBlock());
				builder->CreateBr(leftRecurseBlock);
			}

			if (leftTopTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftTopTypeBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, righttype, &failBlock, &successBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, failBlock);
			}

			if (leftBottomTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftBottomTypeBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, righttype, &failBlock, &failBlock, &failBlock, &successBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, failBlock);
			}

			if (leftDynamicTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftDynamicTypeBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, righttype, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &successBlock, &failBlock, failBlock);
			}

			if (leftMaybeTypeBlock != nullptr)
			{
				BasicBlock* rightMaybeTypeBlock = nullptr;
				builder->SetInsertPoint(leftMaybeTypeBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, righttype, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &rightMaybeTypeBlock, failBlock);
				if (rightMaybeTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightMaybeTypeBlock);
					auto recursiveResult = builder->CreateCall(fun, { RTMaybeType::GenerateReadPotentialType(builder, leftPHI), leftsubsts, RTMaybeType::GenerateReadPotentialType(builder, righttype) });
					builder->CreateCondBr(recursiveResult, successBlock, failBlock);
				}
			}

			{
				BasicBlock* leftClsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "leftClassInfoMerge", fun);
				builder->SetInsertPoint(leftClsMergeBlock);
				auto leftTypeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "leftTypeArgs");
				auto leftIfacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "leftInterfacePtr");

				if (leftClassTypeBlock != nullptr)
				{
					builder->SetInsertPoint(leftClassTypeBlock);
					leftTypeArgsPHI->addIncoming(RTClassType::GetTypeArgumentsPtr(builder, leftPHI), builder->GetInsertBlock());
					leftIfacePHI->addIncoming(RTClassType::GenerateReadClassDescriptorLink(builder, leftPHI), builder->GetInsertBlock());
					builder->CreateBr(leftClsMergeBlock);

				}
				if (leftInstanceTypeBlock != nullptr)
				{
					builder->SetInsertPoint(leftInstanceTypeBlock);
					leftTypeArgsPHI->addIncoming(RTInstanceType::GetTypeArgumentsPtr(builder, leftPHI), builder->GetInsertBlock());
					leftIfacePHI->addIncoming(RTInstanceType::GenerateReadClassDescriptorLink(builder, leftPHI), builder->GetInsertBlock());
					builder->CreateBr(leftClsMergeBlock);
				}

				builder->SetInsertPoint(leftClsMergeBlock);

				BasicBlock* rightClassTypeBlock = nullptr, * rightInstanceTypeBlock = nullptr;
				RTTypeHead::GenerateTypeKindSwitch(builder, righttype, &rightClassTypeBlock, &successBlock, &failBlock, &failBlock, &rightInstanceTypeBlock, nullptr, nullptr, &failBlock, &failBlock, failBlock);

				BasicBlock* rightClsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "rightClassInfoMerge", fun);
				builder->SetInsertPoint(rightClsMergeBlock);
				auto rightTypeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "rightTypeArgs");
				auto rightIfacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "rightInterfacePtr");

				if (rightClassTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightClassTypeBlock);
					rightTypeArgsPHI->addIncoming(RTClassType::GetTypeArgumentsPtr(builder, righttype), builder->GetInsertBlock());
					rightIfacePHI->addIncoming(RTClassType::GenerateReadClassDescriptorLink(builder, righttype), builder->GetInsertBlock());
					builder->CreateBr(rightClsMergeBlock);

				}
				if (rightInstanceTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightInstanceTypeBlock);
					rightTypeArgsPHI->addIncoming(RTInstanceType::GetTypeArgumentsPtr(builder, righttype), builder->GetInsertBlock());
					rightIfacePHI->addIncoming(RTInstanceType::GenerateReadClassDescriptorLink(builder, righttype), builder->GetInsertBlock());
					builder->CreateBr(rightClsMergeBlock);
				}

				BasicBlock* foundNamedTypeMatch = BasicBlock::Create(LLVMCONTEXT, "namedTypesMatch", fun);
				BasicBlock* argCheckLoopHead = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$head", fun);
				BasicBlock* argCheckLoopBody = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$body", fun);
				builder->SetInsertPoint(rightClsMergeBlock);
				builder->CreateCondBr(CreatePointerEq(builder, leftIfacePHI, rightIfacePHI), foundNamedTypeMatch, failBlock);

				builder->SetInsertPoint(foundNamedTypeMatch);
				auto ifaceArgCount = builder->CreateZExtOrTrunc(RTInterface::GenerateReadTypeArgCount(builder, leftIfacePHI), numtype(int32_t));
				auto ifaceArgPos = MakeIntLike(ifaceArgCount, 0);
				auto ifaceArgsLeft = builder->CreateICmpULT(ifaceArgPos, ifaceArgCount);
				builder->CreateCondBr(ifaceArgsLeft, argCheckLoopHead, successBlock);

				builder->SetInsertPoint(argCheckLoopHead);
				PHINode* ifaceArgPosPHI = builder->CreatePHI(ifaceArgPos->getType(), 2);
				ifaceArgPosPHI->addIncoming(ifaceArgPos, foundNamedTypeMatch);
				auto currentLeftArg = MakeLoad(builder, builder->CreateGEP(leftTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto currentRightArg = MakeLoad(builder, builder->CreateGEP(rightTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto argsEqual = builder->CreateCall(fun, { currentLeftArg, leftsubsts, currentRightArg });
				argsEqual->setCallingConv(NOMCC);
				builder->CreateCondBr(argsEqual, argCheckLoopBody, failBlock);

				builder->SetInsertPoint(argCheckLoopBody);
				auto newIndex = builder->CreateAdd(ifaceArgPosPHI, MakeInt32(1));
				ifaceArgsLeft = builder->CreateICmpULT(newIndex, ifaceArgCount);
				ifaceArgPosPHI->addIncoming(newIndex, builder->GetInsertBlock());
				builder->CreateCondBr(ifaceArgsLeft, argCheckLoopHead, successBlock);
			}
			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify RT_NOM_Subtyping!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw new std::exception();
			}
			return fun;
		}

		llvm::Function* RTTypeEq::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_TypeEQ");
		}
	}
}