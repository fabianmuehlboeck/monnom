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
#include "RTCast.h"
#include "RTSubtyping.h"
#include "RTSubstStack.h"
#include "NomTypeVar.h"
#include "NomClassType.h"
#include "NomMaybeType.h"
#include "NullClass.h"
#include "Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		RTTypeEq::RTTypeEq(bool optimistic) : optimistic(optimistic)
		{

		}
		RTTypeEq& RTTypeEq::Instance(bool optimistic)
		{
			static RTTypeEq rtte_opt(true);
			static RTTypeEq rtte_pess(false);
			if (optimistic)
			{
				return rtte_opt;
			}
			else
			{
				return rtte_pess;
			}
		}

		llvm::FunctionType* RTTypeEq::GetLLVMFunctionType(bool optimistic)
		{
			static FunctionType* ft = FunctionType::get(inttype(1), { TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo(), TYPETYPE , RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			static FunctionType* ftopt = FunctionType::get(inttype(2), { TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo(), TYPETYPE , RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			if (optimistic)
			{
				return ftopt;
			}
			else
			{
				return ft;
			}
		}
		llvm::Function* RTTypeEq::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			FunctionType* funtype = GetLLVMFunctionType(optimistic);
			Function* fun = Function::Create(funtype, linkage, optimistic ? "RT_NOM_TypeEQ_OPTIMISTIC" : "RT_NOM_TypeEQ", &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;
			llvm::Argument* arg = fun->arg_begin();
			llvm::Value* lefttype = arg;
			arg++;
			llvm::Value* leftsubsts = arg;
			arg++;
			llvm::Value* righttype = arg;
			arg++;
			llvm::Value* rightsubsts = arg;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "", fun);
			BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "typeEqSuccess", fun);
			BasicBlock* failBlock = BasicBlock::Create(LLVMCONTEXT, "typeEqFail", fun);
			BasicBlock* optimisticSuccessBlock = failBlock;
			if (optimistic)
			{
				optimisticSuccessBlock = BasicBlock::Create(LLVMCONTEXT, "typeEqOptimisticSuccess", fun);
				builder->SetInsertPoint(optimisticSuccessBlock);
				builder->CreateRet(MakeUInt(2, 1));
			}
			BasicBlock* leftRecurseBlock = BasicBlock::Create(LLVMCONTEXT, "leftRecurse", fun);
			builder->SetInsertPoint(startBlock);
			auto areEqual = CreatePointerEq(builder, lefttype, righttype, "typePtrEq");
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { areEqual, MakeUInt(1,1) });
			builder->CreateCondBr(areEqual, successBlock, leftRecurseBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(successBlock);
			if (optimistic)
			{
				builder->CreateRet(MakeUInt(2, 3));
			}
			else
			{
				builder->CreateRet(MakeUInt(1, 1));
			}

			builder->SetInsertPoint(failBlock);
			builder->CreateRet(MakeUInt(optimistic ? 2 : 1, 0));

			builder->SetInsertPoint(leftRecurseBlock);
			Value* leftPHI = nullptr;
			Value* leftSubstPHI = nullptr;

			BasicBlock* leftClassTypeBlock = nullptr, * leftTopTypeBlock = nullptr, * leftTypeVarBlock = nullptr, * leftBottomTypeBlock = nullptr, * leftInstanceTypeBlock = nullptr, * leftDynamicTypeBlock = nullptr, * leftMaybeTypeBlock = nullptr;

			RTTypeHead::GenerateTypeKindSwitchRecurse(builder, lefttype, leftsubsts, &leftPHI, &leftSubstPHI, &leftClassTypeBlock, &leftTopTypeBlock, &leftTypeVarBlock, &leftBottomTypeBlock, &leftInstanceTypeBlock, &leftDynamicTypeBlock, &leftMaybeTypeBlock, failBlock);

			if (leftTypeVarBlock != nullptr)
			{
				BasicBlock* rightTypeVarBlock = nullptr;
				builder->SetInsertPoint(leftTypeVarBlock);
				Value* rightPHI = nullptr;
				Value* rightSubstPHI = nullptr;
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, &rightPHI, &rightSubstPHI, &failBlock, &failBlock, &rightTypeVarBlock, &failBlock, &failBlock, &optimisticSuccessBlock, &failBlock, failBlock);

				if (rightTypeVarBlock != nullptr)
				{
					builder->SetInsertPoint(rightTypeVarBlock);
					auto varsMatch = CreatePointerEq(builder, leftPHI, rightPHI, "typeVariablesMatch");
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { varsMatch, MakeUInt(1,1) });
					builder->CreateCondBr(varsMatch, successBlock, failBlock, GetLikelyFirstBranchMetadata());
				}
			}

			if (leftTopTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftTopTypeBlock);
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, nullptr, nullptr, &failBlock, &successBlock, &failBlock, &failBlock, &failBlock, &optimisticSuccessBlock, &failBlock, failBlock);
			}

			if (leftBottomTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftBottomTypeBlock);
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, nullptr, nullptr, &failBlock, &failBlock, &failBlock, &successBlock, &failBlock, &optimisticSuccessBlock, &failBlock, failBlock);
			}

			if (leftDynamicTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftDynamicTypeBlock);
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, nullptr, nullptr, &optimisticSuccessBlock, &optimisticSuccessBlock, &optimisticSuccessBlock, &optimisticSuccessBlock, &optimisticSuccessBlock, &successBlock, &optimisticSuccessBlock, failBlock);
			}

			if (leftMaybeTypeBlock != nullptr)
			{
				BasicBlock* rightMaybeTypeBlock = nullptr;
				builder->SetInsertPoint(leftMaybeTypeBlock);
				Value* rightPHI = nullptr;
				Value* rightSubstPHI = nullptr;

				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, &rightPHI, &rightSubstPHI, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &optimisticSuccessBlock, &rightMaybeTypeBlock, failBlock);

				if (rightMaybeTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightMaybeTypeBlock);
					auto recursiveResult = builder->CreateCall(fun, { RTMaybeType::GenerateReadPotentialType(builder, leftPHI), leftSubstPHI, RTMaybeType::GenerateReadPotentialType(builder, rightPHI), rightSubstPHI });
					recursiveResult->setCallingConv(fun->getCallingConv());
					if (optimistic)
					{
						auto resultSwitch = builder->CreateSwitch(recursiveResult, failBlock, 2, GetBranchWeights({0,100,50}));
						resultSwitch->addCase(MakeUInt(2, 3), successBlock);
						resultSwitch->addCase(MakeUInt(2, 1), optimisticSuccessBlock);
					}
					else
					{
						builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { recursiveResult, MakeUInt(1,1) });
						builder->CreateCondBr(recursiveResult, successBlock, failBlock, GetLikelyFirstBranchMetadata());
					}
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
				Value* rightPHI = nullptr;
				Value* rightSubstPHI = nullptr;

				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, righttype, rightsubsts, &rightPHI, &rightSubstPHI, &rightClassTypeBlock, &failBlock, &failBlock, &failBlock, &rightInstanceTypeBlock, &optimisticSuccessBlock, &failBlock, failBlock);

				BasicBlock* rightClsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "rightClassInfoMerge", fun);
				builder->SetInsertPoint(rightClsMergeBlock);
				auto rightTypeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "rightTypeArgs");
				auto rightIfacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "rightInterfacePtr");

				if (rightClassTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightClassTypeBlock);
					rightTypeArgsPHI->addIncoming(RTClassType::GetTypeArgumentsPtr(builder, rightPHI), builder->GetInsertBlock());
					rightIfacePHI->addIncoming(RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI), builder->GetInsertBlock());
					builder->CreateBr(rightClsMergeBlock);

				}
				if (rightInstanceTypeBlock != nullptr)
				{
					builder->SetInsertPoint(rightInstanceTypeBlock);
					rightTypeArgsPHI->addIncoming(RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), builder->GetInsertBlock());
					rightIfacePHI->addIncoming(RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI), builder->GetInsertBlock());
					builder->CreateBr(rightClsMergeBlock);
				}

				BasicBlock* foundNamedTypeMatch = BasicBlock::Create(LLVMCONTEXT, "namedTypesMatch", fun);
				BasicBlock* argCheckBlock = BasicBlock::Create(LLVMCONTEXT, "argCheck", fun);
				BasicBlock* argCheckLoopHead = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$head", fun);
				BasicBlock* argCheckLoopBody = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$body", fun);
				builder->SetInsertPoint(rightClsMergeBlock);
				auto ifacesMatch = CreatePointerEq(builder, leftIfacePHI, rightIfacePHI);
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { ifacesMatch, MakeUInt(1,1) });
				builder->CreateCondBr(ifacesMatch, foundNamedTypeMatch, failBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(foundNamedTypeMatch);
				auto ifaceArgCount = builder->CreateZExtOrTrunc(RTInterface::GenerateReadTypeArgCount(builder, leftIfacePHI), numtype(int32_t));
				auto ifaceArgPos = MakeIntLike(ifaceArgCount, 0);
				auto ifaceArgsLeft = builder->CreateICmpULT(ifaceArgPos, ifaceArgCount);
				builder->CreateCondBr(ifaceArgsLeft, argCheckBlock, successBlock);

				builder->SetInsertPoint(argCheckBlock);
				BasicBlock* releaseSuccessBlock = nullptr, * releaseSuccessBlockX = nullptr;
				BasicBlock* releaseFailBlock = nullptr, * releaseFailBlockX = nullptr;
				RTSubstStackValue argCheckLeftSubsts = RTSubstStackValue(builder, leftTypeArgsPHI, leftSubstPHI);
				RTSubstStackValue argCheckRightSubsts = RTSubstStackValue(builder, rightTypeArgsPHI, rightSubstPHI);
				argCheckRightSubsts.MakeReleaseBlocks(builder, successBlock, &releaseSuccessBlockX, failBlock, &releaseFailBlockX);
				argCheckLeftSubsts.MakeReleaseBlocks(builder, releaseSuccessBlockX, &releaseSuccessBlock, releaseFailBlockX, &releaseFailBlock);
				builder->CreateBr(argCheckLoopHead);

				builder->SetInsertPoint(argCheckLoopHead);
				PHINode* ifaceArgPosPHI = builder->CreatePHI(ifaceArgPos->getType(), 2);
				ifaceArgPosPHI->addIncoming(ifaceArgPos, argCheckBlock);
				auto currentLeftArg = MakeLoad(builder, builder->CreateGEP(leftTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto currentRightArg = MakeLoad(builder, builder->CreateGEP(rightTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto argsEqual = builder->CreateCall(fun, { currentLeftArg, argCheckLeftSubsts, currentRightArg, argCheckRightSubsts });
				argsEqual->setCallingConv(NOMCC);
				if (optimistic)
				{
					auto resultSwitch = builder->CreateSwitch(argsEqual, failBlock, 2, GetBranchWeights({ 1,100,50 }));
					resultSwitch->addCase(MakeUInt(2, 3), successBlock);
					resultSwitch->addCase(MakeUInt(2, 1), optimisticSuccessBlock);
				}
				else
				{
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { argsEqual, MakeUInt(1,1) });
					builder->CreateCondBr(argsEqual, argCheckLoopBody, releaseFailBlock, GetLikelyFirstBranchMetadata());
				}

				builder->SetInsertPoint(argCheckLoopBody);
				auto newIndex = builder->CreateAdd(ifaceArgPosPHI, MakeInt32(1));
				ifaceArgsLeft = builder->CreateICmpULT(newIndex, ifaceArgCount);
				ifaceArgPosPHI->addIncoming(newIndex, builder->GetInsertBlock());
				builder->CreateCondBr(ifaceArgsLeft, argCheckLoopHead, releaseSuccessBlock);
			}
			llvm::raw_os_ostream out(std::cout);
			if (verifyFunction(*fun, &out))
			{
				out.flush();
				std::cout << "Could not verify RT_NOM_TypeEQ!";
				fun->print(out);
				out.flush();
				std::cout.flush();
				throw new std::exception();
			}
			return fun;
		}

		void RTTypeEq::CreateInlineTypeEqCheck(NomBuilder& builder, llvm::Value* leftType, NomTypeRef rightType, llvm::Value* leftsubstitutions, llvm::Value* rightsubstitutions, BasicBlock* pessimisticBlock, BasicBlock* optimisticBlock, BasicBlock* failBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			switch (rightType->GetKind())
			{
			case TypeKind::TKDynamic:
				builder->CreateBr(pessimisticBlock);
				break;
			case TypeKind::TKTop:
			{
				if (optimisticBlock == nullptr)
				{
					optimisticBlock = failBlock;
				}
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, leftType, leftsubstitutions, nullptr, nullptr, &failBlock, &pessimisticBlock, &failBlock, &failBlock, &failBlock, &optimisticBlock, &failBlock, failBlock);
			}
			break;
			case TypeKind::TKBottom:
			{
				if (optimisticBlock == nullptr)
				{
					optimisticBlock = failBlock;
				}
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, leftType, leftsubstitutions, nullptr, nullptr, &failBlock, &failBlock, &failBlock, &pessimisticBlock, &failBlock, &optimisticBlock, &failBlock, failBlock);
			}
			break;
			case TypeKind::TKVariable:
			{
				auto rightTypeArgs = MakeInvariantLoad(builder, rightsubstitutions, MakeInt32(TypeArgumentListStackFields::Types));
				auto rightTypeSubst = MakeInvariantLoad(builder, builder->CreateGEP(rightTypeArgs, MakeInt32(-(((NomTypeVarRef)rightType)->GetIndex() + 1))));
				auto restRightSubsts = MakeInvariantLoad(builder, rightsubstitutions, MakeInt32(TypeArgumentListStackFields::Next));
				Function* typeEqFun = RTTypeEq::Instance(optimisticBlock != nullptr && optimisticBlock != pessimisticBlock).GetLLVMElement(*fun->getParent());
				auto typeEqResult = builder->CreateCall(typeEqFun, { leftType, leftsubstitutions, rightTypeSubst, restRightSubsts });
				typeEqResult->setCallingConv(NOMCC);
				if (optimisticBlock == nullptr || optimisticBlock == pessimisticBlock)
				{
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { typeEqResult, MakeUInt(1,1) });
					builder->CreateCondBr(typeEqResult, pessimisticBlock, failBlock, GetLikelyFirstBranchMetadata());
				}
				else
				{
					auto successSwitch = builder->CreateSwitch(typeEqResult, failBlock, 2, GetBranchWeights({ 1,100,50 }));
					successSwitch->addCase(MakeUInt(2, 3), pessimisticBlock);
					successSwitch->addCase(MakeUInt(2, 1), optimisticBlock);
				}
			}
			break;
			case TypeKind::TKClass:
			{
				Value* leftPHI = nullptr;
				Value* leftSubstPHI = nullptr;
				auto clsType = (NomClassTypeRef)rightType;
				BasicBlock* leftClassTypeBlock = nullptr, * leftInstanceTypeBlock = nullptr;
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, leftType, leftsubstitutions, &leftPHI, &leftSubstPHI, &leftClassTypeBlock, &failBlock, &failBlock, &failBlock, &leftInstanceTypeBlock, (optimisticBlock == nullptr ? &failBlock : &optimisticBlock), &failBlock, failBlock);

				BasicBlock* clsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "classInfoMerge", fun);
				builder->SetInsertPoint(clsMergeBlock);
				auto typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "typeArgs");
				auto ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "interfacePtr");

				if (leftClassTypeBlock != nullptr)
				{
					builder->SetInsertPoint(leftClassTypeBlock);
					typeArgsPHI->addIncoming(RTClassType::GetTypeArgumentsPtr(builder, leftPHI), builder->GetInsertBlock());
					ifacePHI->addIncoming(RTClassType::GenerateReadClassDescriptorLink(builder, leftPHI), builder->GetInsertBlock());
					builder->CreateBr(clsMergeBlock);

				}
				if (leftInstanceTypeBlock != nullptr)
				{
					builder->SetInsertPoint(leftInstanceTypeBlock);
					typeArgsPHI->addIncoming(RTInstanceType::GetTypeArgumentsPtr(builder, leftPHI), builder->GetInsertBlock());
					ifacePHI->addIncoming(RTInstanceType::GenerateReadClassDescriptorLink(builder, leftPHI), builder->GetInsertBlock());
					builder->CreateBr(clsMergeBlock);
				}

				builder->SetInsertPoint(clsMergeBlock);
				BasicBlock* foundNamedTypeMatch = BasicBlock::Create(LLVMCONTEXT, "foundNamedTypeMatch", fun);
				auto namedTypeMatch = builder->CreateICmpEQ(ConstantExpr::getPtrToInt(clsType->Named->GetInterfaceDescriptor(*fun->getParent()), numtype(intptr_t)), builder->CreatePtrToInt(ifacePHI, numtype(intptr_t)));
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { namedTypeMatch, MakeUInt(1,1) });
				builder->CreateCondBr(namedTypeMatch, foundNamedTypeMatch, failBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(foundNamedTypeMatch);
				int argIndex = 0;
				BasicBlock* nextCheck = builder->GetInsertBlock();
				BasicBlock* nextOptCheck = nullptr;
				for (auto* targ : clsType->Arguments)
				{
					argIndex++; //needs to be at least -1 anyway
					BasicBlock* curOptCheck = nextOptCheck;
					nextCheck = BasicBlock::Create(LLVMCONTEXT, "typeArg" + std::to_string(argIndex), fun);
					if (optimisticBlock != nullptr && optimisticBlock != pessimisticBlock)
					{
						nextOptCheck = BasicBlock::Create(LLVMCONTEXT, "typeArgOpt" + std::to_string(argIndex), fun);
					}
					auto argLeft = MakeInvariantLoad(builder, builder->CreateGEP(typeArgsPHI, MakeInt32(-argIndex)));
					CreateInlineTypeEqCheck(builder, argLeft, targ, leftSubstPHI, rightsubstitutions, nextCheck, optimisticBlock == pessimisticBlock ? nextCheck : nextOptCheck, failBlock);

					if (curOptCheck != nullptr)
					{
						builder->SetInsertPoint(curOptCheck);
						auto argLeft = MakeInvariantLoad(builder, builder->CreateGEP(typeArgsPHI, MakeInt32(-argIndex)));
						CreateInlineTypeEqCheck(builder, argLeft, targ, leftSubstPHI, rightsubstitutions, nextOptCheck, nextOptCheck, failBlock);
					}

					builder->SetInsertPoint(nextCheck);
				}
				builder->CreateBr(pessimisticBlock);

				if (nextOptCheck != nullptr)
				{
					builder->SetInsertPoint(nextOptCheck);
					builder->CreateBr(optimisticBlock);
				}
			}
			break;
			case TypeKind::TKMaybe:
			{
				if (optimisticBlock == nullptr)
				{
					optimisticBlock = failBlock;
				}
				BasicBlock* maybeBlock = nullptr;
				Value* leftPHI = nullptr;
				Value* leftSubstPHI = nullptr;
				RTTypeHead::GenerateTypeKindSwitchRecurse(builder, leftType, leftsubstitutions, &leftPHI, &leftSubstPHI, &failBlock, &failBlock, &failBlock, &failBlock, &failBlock, &optimisticBlock, &maybeBlock, failBlock);

				if (maybeBlock != nullptr)
				{
					builder->SetInsertPoint(maybeBlock);
					RTTypeEq::CreateInlineTypeEqCheck(builder, leftPHI, ((NomMaybeTypeRef)rightType)->PotentialType, leftSubstPHI, rightsubstitutions, pessimisticBlock, optimisticBlock, failBlock);
				}
			}
			default:
				throw new std::exception();
			}
		}

		llvm::Function* RTTypeEq::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction(optimistic ? "RT_NOM_TypeEQ_OPTIMISTIC" : "RT_NOM_TypeEQ");
		}
	}
}