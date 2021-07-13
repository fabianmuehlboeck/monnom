#include "RTSubtyping.h"
#include "Defs.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "RTTypeHead.h"
#include "RTClassType.h"
#include "RTTypeVar.h"
#include "NomClassType.h"
#include "RTInstanceType.h"
#include "RTDisjointness.h"
#include "RTFilterInstantiations.h"
#include "NomInterface.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/Verifier.h"
#include "CompileHelpers.h"
#include "RTConfig.h"
#include "CastStats.h"
#include "RTInterface.h"
#include "CallingConvConf.h"
#include "RTCast.h"
#include "NullClass.h"
#include "NomMaybeType.h"
#include "NomTypeVar.h"
#include "RTTypeEq.h"
#include "RTMaybeType.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		RTSubtyping& RTSubtyping::Instance()
		{
			static RTSubtyping rts; return rts;
		}

		RTSubtyping::RTSubtyping()
		{

		}

		RTSubtyping::~RTSubtyping()
		{
		}


		llvm::Function* RTSubtyping::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			llvm::FunctionType* funType = SubtypingFunctionType();
			llvm::Function* fun = llvm::Function::Create(funType, linkage, "NOM_RT_Subtyping", &mod);
			fun->setCallingConv(NOMCC);
			llvm::Argument* left = fun->arg_begin();
			llvm::Argument* right = left + 1;
			llvm::Argument* leftsubst = left + 2;
			llvm::Argument* rightsubst = left + 3;
			NomBuilder builder;
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "start", fun);
			BasicBlock* leftUnfoldBlock = BasicBlock::Create(LLVMCONTEXT, "leftVariableUnfold", fun);
			BasicBlock* successBlock = BasicBlock::Create(LLVMCONTEXT, "subtypingSuccess", fun);
			BasicBlock* optimisticBlock = BasicBlock::Create(LLVMCONTEXT, "subtypingOptimisticSuccess", fun);
			BasicBlock* failBlock = BasicBlock::Create(LLVMCONTEXT, "subtypingFail", fun);
			builder->SetInsertPoint(startBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncSubtypingChecksFunction(mod), {});
			}
			auto argsEqual = builder->CreateICmpEQ(builder->CreatePtrToInt(left, numtype(intptr_t)), builder->CreatePtrToInt(right, numtype(intptr_t)));
			argsEqual = builder->CreateAnd({ argsEqual, builder->CreateIsNull(leftsubst), builder->CreateIsNull(rightsubst) });
			builder->CreateCondBr(argsEqual, successBlock, leftUnfoldBlock);

			builder->SetInsertPoint(leftUnfoldBlock);
			PHINode* leftPHI = builder->CreatePHI(left->getType(), 2);
			PHINode* leftSubstPHI = builder->CreatePHI(leftsubst->getType(), 2);
			leftPHI->addIncoming(left, startBlock);
			leftSubstPHI->addIncoming(leftsubst, startBlock);


			BasicBlock* leftClassTypeBlock = nullptr, * leftTypeVarBlock = nullptr, * leftTopTypeBlock = nullptr, * leftInstanceTypeBlock = nullptr, * leftMaybeTypeBlock = nullptr, * leftDynamicTypeBlock = nullptr;
			RTTypeHead::GenerateTypeKindSwitch(builder, leftPHI, &leftClassTypeBlock, &leftTopTypeBlock, &leftTypeVarBlock, &successBlock, &leftInstanceTypeBlock, nullptr, nullptr, &leftDynamicTypeBlock, &leftMaybeTypeBlock);

			if (leftTypeVarBlock != nullptr)
			{
				builder->SetInsertPoint(leftTypeVarBlock);
				auto leftTypeArr = MakeLoad(builder, leftSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
				auto newLeft = MakeLoad(builder, builder->CreateGEP(leftTypeArr, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, leftPHI))));
				auto newsubst = MakeLoad(builder, leftSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Next));
				leftPHI->addIncoming(newLeft, builder->GetInsertBlock());
				leftSubstPHI->addIncoming(newsubst, builder->GetInsertBlock());
				builder->CreateBr(leftUnfoldBlock);
			}

			if (leftTopTypeBlock != nullptr)
			{
				BasicBlock* rightTypeVarBlock = nullptr;
				builder->SetInsertPoint(leftTopTypeBlock);
				auto rightPHI = builder->CreatePHI(right->getType(), 2);
				auto rightSubstPHI = builder->CreatePHI(rightsubst->getType(), 2);
				rightPHI->addIncoming(right, leftUnfoldBlock);
				rightSubstPHI->addIncoming(rightsubst, leftUnfoldBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, nullptr, &successBlock, &rightTypeVarBlock, nullptr, nullptr, nullptr, nullptr, &successBlock, nullptr, failBlock);

				if (rightTypeVarBlock != nullptr)
				{
					builder->SetInsertPoint(rightTypeVarBlock);
					auto rightTypeArr = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
					auto newRight = MakeLoad(builder, builder->CreateGEP(rightTypeArr, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
					auto newsubst = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Next));
					rightPHI->addIncoming(newRight, builder->GetInsertBlock());
					rightSubstPHI->addIncoming(newsubst, builder->GetInsertBlock());
					builder->CreateBr(leftTopTypeBlock);
				}
			}

			if (leftMaybeTypeBlock != nullptr)
			{
				BasicBlock* superOfNullBlock = BasicBlock::Create(LLVMCONTEXT, "rightSuperOfNull", fun);
				builder->SetInsertPoint(leftMaybeTypeBlock);
				auto superOfNull = builder->CreateCall(fun, { NomNullClass::GetInstance()->GetType()->GetLLVMElement(mod), right, ConstantPointerNull::get(TypeArgumentListStackType()->getPointerTo()), rightsubst });
				superOfNull->setCallingConv(NOMCC);
				builder->CreateCondBr(builder->CreateICmpEQ(superOfNull, MakeUInt(2, 3)), superOfNullBlock, failBlock);

				builder->SetInsertPoint(superOfNullBlock);
				auto superOfOther = builder->CreateCall(fun, { RTMaybeType::GenerateReadPotentialType(builder, leftPHI), right, leftSubstPHI, rightsubst });
				superOfOther->setCallingConv(NOMCC);
				builder->CreateCondBr(builder->CreateICmpEQ(superOfOther, MakeUInt(2, 3)), successBlock, failBlock);
			}

			if (leftDynamicTypeBlock != nullptr)
			{
				builder->SetInsertPoint(leftDynamicTypeBlock);
				BasicBlock* rightTypeVarBlock = nullptr;
				auto rightPHI = builder->CreatePHI(right->getType(), 2);
				auto rightSubstPHI = builder->CreatePHI(rightsubst->getType(), 2);
				rightPHI->addIncoming(right, leftUnfoldBlock);
				rightSubstPHI->addIncoming(rightsubst, leftUnfoldBlock);
				RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, &optimisticBlock, &successBlock, &rightTypeVarBlock, &optimisticBlock, &optimisticBlock, nullptr, nullptr, &successBlock, &optimisticBlock, failBlock);

				if (rightTypeVarBlock != nullptr)
				{
					builder->SetInsertPoint(rightTypeVarBlock);
					auto rightTypeArr = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
					auto newRight = MakeLoad(builder, builder->CreateGEP(rightTypeArr, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
					auto newsubst = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Next));
					rightPHI->addIncoming(newRight, builder->GetInsertBlock());
					rightSubstPHI->addIncoming(newsubst, builder->GetInsertBlock());
					builder->CreateBr(leftDynamicTypeBlock);
				}
			}

			//now on to the two versions of class types
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
				BasicBlock* rightMerge = BasicBlock::Create(LLVMCONTEXT, "rightVarUnfold", fun);
				builder->CreateBr(rightMerge);

				builder->SetInsertPoint(rightMerge);
				auto rightPHI = builder->CreatePHI(right->getType(), 2);
				auto rightSubstPHI = builder->CreatePHI(rightsubst->getType(), 2);
				rightPHI->addIncoming(right, leftClsMergeBlock);
				rightSubstPHI->addIncoming(rightsubst, leftClsMergeBlock);

				BasicBlock* rightTypeVarBlock = nullptr, * rightClassTypeBlock = nullptr, * rightInstanceTypeBlock = nullptr, * rightMaybeTypeBlock = nullptr;
				RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, &rightClassTypeBlock, &successBlock, &rightTypeVarBlock, &failBlock, &rightInstanceTypeBlock, nullptr, nullptr, &successBlock, &rightMaybeTypeBlock, failBlock);

				if (rightTypeVarBlock != nullptr)
				{
					builder->SetInsertPoint(rightTypeVarBlock);
					auto rightTypeArr = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
					auto newRight = MakeLoad(builder, builder->CreateGEP(rightTypeArr, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
					auto newsubst = MakeLoad(builder, rightSubstPHI, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Next));
					rightPHI->addIncoming(newRight, builder->GetInsertBlock());
					rightSubstPHI->addIncoming(newsubst, builder->GetInsertBlock());
					builder->CreateBr(rightMerge);
				}

				if (rightMaybeTypeBlock != nullptr)
				{
					BasicBlock* notSubOfNullBlock = BasicBlock::Create(LLVMCONTEXT, "notSubOfNull", fun);
					builder->SetInsertPoint(rightMaybeTypeBlock);
					auto subOfNull = builder->CreateCall(fun, { leftPHI, NomNullClass::GetInstance()->GetType()->GetLLVMElement(mod), leftSubstPHI, ConstantPointerNull::get(TypeArgumentListStackType()->getPointerTo()) });
					subOfNull->setCallingConv(NOMCC);
					builder->CreateCondBr(builder->CreateICmpEQ(subOfNull, MakeUInt(2, 3)), successBlock, notSubOfNullBlock);

					builder->SetInsertPoint(notSubOfNullBlock);
					auto subOfOther = builder->CreateCall(fun, { leftPHI,  RTMaybeType::GenerateReadPotentialType(builder, rightPHI), leftSubstPHI, rightSubstPHI });
					subOfOther->setCallingConv(NOMCC);
					builder->CreateCondBr(builder->CreateICmpEQ(subOfOther, MakeUInt(2, 3)), successBlock, failBlock);
				}

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

				builder->SetInsertPoint(rightClsMergeBlock);

				BasicBlock* foundNamedImmediateTypeMatch = BasicBlock::Create(LLVMCONTEXT, "foundNamedImmediateTypeMatch", fun);
				BasicBlock* foundNamedImmediateTypeMatchLoadSubsts = BasicBlock::Create(LLVMCONTEXT, "foundNamedImmediateTypeMatch$loadSubsts", fun);
				BasicBlock* foundNamedTypeMatch = BasicBlock::Create(LLVMCONTEXT, "foundNamedTypeMatch", fun);
				BasicBlock* foundNamedTypeMatchLoad = BasicBlock::Create(LLVMCONTEXT, "foundNamedTypeMatch$Load", fun);
				BasicBlock* superTypeLoopHead = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Head", fun);
				BasicBlock* superTypeLoopBody = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Body", fun);
				BasicBlock* superTypeLoopBodyEnd = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Body$End", fun);
				auto namedTypeMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(leftIfacePHI, numtype(intptr_t)), builder->CreatePtrToInt(rightIfacePHI, numtype(intptr_t)));
				builder->CreateCondBr(namedTypeMatch, foundNamedImmediateTypeMatch, superTypeLoopHead);

				builder->SetInsertPoint(foundNamedImmediateTypeMatch);
				builder->CreateCondBr(builder->CreateIsNull(leftSubstPHI), foundNamedTypeMatch, foundNamedImmediateTypeMatchLoadSubsts);

				builder->SetInsertPoint(foundNamedImmediateTypeMatchLoadSubsts);
				auto leftSubstArr = MakeLoad(builder, leftSubstPHI, MakeInt32(TypeArgumentListStackFields::Types));
				builder->CreateBr(foundNamedTypeMatch);

				builder->SetInsertPoint(foundNamedTypeMatch);
				BasicBlock* argCheckLoopHead = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$Head", fun);
				BasicBlock* argCheckLoopBody = BasicBlock::Create(LLVMCONTEXT, "argCheckLoop$Body", fun);

				PHINode* foundMatchTypeArgsPHI = builder->CreatePHI(leftTypeArgsPHI->getType(), 3);
				PHINode* foundSubstitutionsPHI = builder->CreatePHI(leftSubstArr->getType(), 3);
				foundMatchTypeArgsPHI->addIncoming(leftTypeArgsPHI, foundNamedImmediateTypeMatchLoadSubsts);
				foundSubstitutionsPHI->addIncoming(leftSubstArr, foundNamedImmediateTypeMatchLoadSubsts);
				foundMatchTypeArgsPHI->addIncoming(leftTypeArgsPHI, foundNamedImmediateTypeMatch);
				foundSubstitutionsPHI->addIncoming(ConstantPointerNull::get(TYPETYPE->getPointerTo()), foundNamedImmediateTypeMatch);
				auto ifaceArgCount = builder->CreateZExtOrTrunc(RTInterface::GenerateReadTypeArgCount(builder, leftIfacePHI), numtype(int32_t));
				auto ifaceArgPos = MakeIntLike(ifaceArgCount, 0);
				auto ifaceArgsLeft = builder->CreateICmpULT(ifaceArgPos, ifaceArgCount);
				builder->CreateCondBr(ifaceArgsLeft, argCheckLoopHead, successBlock);

				builder->SetInsertPoint(argCheckLoopHead);
				PHINode* ifaceArgPosPHI = builder->CreatePHI(ifaceArgPos->getType(), 2);
				ifaceArgPosPHI->addIncoming(ifaceArgPos, foundNamedTypeMatch);
				auto currentLeftArg = MakeLoad(builder, builder->CreateGEP(foundMatchTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto currentRightArg = MakeLoad(builder, builder->CreateGEP(rightTypeArgsPHI, builder->CreateSub(MakeInt32(-1), ifaceArgPosPHI)));
				auto argsEqual = builder->CreateCall(RTTypeEq::Instance().GetLLVMElement(mod), { currentLeftArg, foundSubstitutionsPHI, currentRightArg });
				argsEqual->setCallingConv(NOMCC);
				builder->CreateCondBr(argsEqual, argCheckLoopBody, failBlock);

				{
					builder->SetInsertPoint(argCheckLoopBody);
					auto newIndex = builder->CreateAdd(ifaceArgPosPHI, MakeInt32(1));
					ifaceArgsLeft = builder->CreateICmpULT(newIndex, ifaceArgCount);
					ifaceArgPosPHI->addIncoming(newIndex, builder->GetInsertBlock());
					builder->CreateCondBr(ifaceArgsLeft, argCheckLoopHead, successBlock);
				}
				builder->SetInsertPoint(superTypeLoopHead);
				auto superInstances = RTInterface::GenerateReadSuperInstances(builder, leftIfacePHI);
				auto superInstanceCount = RTInterface::GenerateReadSuperInstanceCount(builder, leftIfacePHI);
				auto currentIndex = MakeIntLike(superInstanceCount, 0);
				auto areInstancesLeft = builder->CreateICmpULT(currentIndex, superInstanceCount);
				builder->CreateCondBr(areInstancesLeft, superTypeLoopBody, failBlock);

				builder->SetInsertPoint(superTypeLoopBody);
				auto indexPHI = builder->CreatePHI(currentIndex->getType(), 2, "superInstanceIndex");
				indexPHI->addIncoming(currentIndex, superTypeLoopHead);
				auto currentSuperIface = MakeLoad(builder, builder->CreateGEP(superInstances, { indexPHI, MakeInt32(SuperInstanceEntryFields::Class) }));
				namedTypeMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(rightIfacePHI, numtype(intptr_t)), builder->CreatePtrToInt(currentSuperIface, numtype(intptr_t)));
				builder->CreateCondBr(namedTypeMatch, foundNamedTypeMatchLoad, superTypeLoopBodyEnd);

				builder->SetInsertPoint(foundNamedTypeMatchLoad);
				auto superInstanceTypeArgs = MakeLoad(builder, builder->CreateGEP(superInstances, { indexPHI, MakeInt32(SuperInstanceEntryFields::TypeArgs) }));
				foundMatchTypeArgsPHI->addIncoming(superInstanceTypeArgs, builder->GetInsertBlock());
				foundSubstitutionsPHI->addIncoming(leftTypeArgsPHI, builder->GetInsertBlock());
				builder->CreateBr(foundNamedTypeMatch);

				{
					builder->SetInsertPoint(superTypeLoopBodyEnd);
					auto newIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1), "newIndex");
					indexPHI->addIncoming(newIndex, superTypeLoopBodyEnd);
					areInstancesLeft = builder->CreateICmpULT(newIndex, superInstanceCount);
					builder->CreateCondBr(areInstancesLeft, superTypeLoopBody, failBlock);
				}
			}

			builder->SetInsertPoint(successBlock);
			builder->CreateRet(MakeUInt(2, 3));

			builder->SetInsertPoint(optimisticBlock);
			builder->CreateRet(MakeUInt(2, 1));

			builder->SetInsertPoint(failBlock);
			builder->CreateRet(MakeUInt(2, 0));

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



		llvm::Function* RTSubtyping::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("NOM_RT_Subtyping");
		}

		//Arguments:
		//left type
		//right type,
		//left type argument list stack,
		//right type argument list stack
		llvm::FunctionType* RTSubtyping::SubtypingFunctionType()
		{
			static llvm::FunctionType* ft = llvm::FunctionType::get(inttype(2), { TYPETYPE, TYPETYPE, TypeArgumentListStackType()->getPointerTo(), TypeArgumentListStackType()->getPointerTo() }, false);
			return ft;
		}

		llvm::StructType* RTSubtyping::TypeArgumentListStackType()
		{
			static llvm::StructType* talst = llvm::StructType::create(LLVMCONTEXT, "NOM_RT_TypeArgumentListStack");
			static bool once = true;
			if (once)
			{
				once = false;
				talst->setBody(talst->getPointerTo(), TYPETYPE->getPointerTo());
			}
			return talst;
		}

		llvm::Value* RTSubtyping::CreateTypeSubtypingCheck(NomBuilder& builder, llvm::Module& mod, llvm::Value* left, llvm::Value* right, llvm::Value* leftsubstitutions, llvm::Value* rightsubstitutions)
		{
			auto stfun = RTSubtyping::Instance().GetLLVMElement(mod);
			auto callinst = builder->CreateCall(stfun, { left, right, leftsubstitutions, rightsubstitutions }, "isSubtype");
			callinst->setCallingConv(NOMCC);
			return callinst;
		}

		static void CreateInlineTypeEqCheck(NomBuilder& builder, llvm::Value* leftType, NomTypeRef rightType, llvm::Value* leftsubstitutions, llvm::Value* rightsubstitutions, BasicBlock* successBlock, BasicBlock* failBlock, llvm::BasicBlock* errorBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			switch (rightType->GetKind())
			{
			case TypeKind::TKDynamic:
				builder->CreateBr(successBlock);
				break;
			case TypeKind::TKTop:
			{
				RTTypeHead::GenerateTypeKindSwitch(builder, leftType, nullptr, &successBlock, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, failBlock);
			}
			break;
			case TypeKind::TKBottom:
			{
				RTTypeHead::GenerateTypeKindSwitch(builder, leftType, nullptr, nullptr, nullptr, &successBlock, nullptr, nullptr, nullptr, nullptr, nullptr, failBlock);
			}
			break;
			case TypeKind::TKVariable:
			{
				auto rightTypeArgs = MakeLoad(builder, rightsubstitutions, MakeInt32(TypeArgumentListStackFields::Types));
				auto rightTypeSubst = MakeLoad(builder, builder->CreateGEP(rightTypeArgs, MakeInt32(-(((NomTypeVarRef)rightType)->GetIndex() + 1))));
				Function* typeEqFun = RTTypeEq::Instance().GetLLVMElement(*fun->getParent());
				auto eqLeftSubsts = GenerateGetTypeArgumentListStackTypes(builder, leftsubstitutions);
				auto typeEqResult = builder->CreateCall(typeEqFun, { leftType, eqLeftSubsts , rightTypeSubst });
				typeEqResult->setCallingConv(NOMCC);
				builder->CreateCondBr(typeEqResult, successBlock, failBlock);
			}
			break;
			case TypeKind::TKClass:
			{
				BasicBlock* leftResolveBlock = BasicBlock::Create(LLVMCONTEXT, "leftVarResolve", fun);
				builder->CreateBr(leftResolveBlock);
				builder->SetInsertPoint(leftResolveBlock);
				auto leftPHI = builder->CreatePHI(leftType->getType(), 2);
				auto leftSubstPHI = builder->CreatePHI(leftsubstitutions ->getType(), 2);
				leftPHI->addIncoming(leftType, origBlock);
				leftSubstPHI->addIncoming(leftsubstitutions, origBlock);
				auto clsType = (NomClassTypeRef)rightType;
				BasicBlock* leftClassTypeBlock = nullptr, * leftInstanceTypeBlock = nullptr, *leftTypeVarBlock=nullptr;
				RTTypeHead::GenerateTypeKindSwitch(builder, leftPHI, &leftClassTypeBlock, nullptr, &leftTypeVarBlock, nullptr, &leftInstanceTypeBlock, nullptr, nullptr, nullptr, nullptr, failBlock);

				BasicBlock* clsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "classInfoMerge", fun);
				builder->SetInsertPoint(clsMergeBlock);
				auto typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "typeArgs");
				auto ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "interfacePtr");

				if (leftTypeVarBlock != nullptr)
				{
					builder->SetInsertPoint(leftTypeVarBlock);
					auto leftTypeArgs = MakeLoad(builder, leftSubstPHI, MakeInt32(TypeArgumentListStackFields::Types));
					auto typeVarIndex = RTTypeVar::GenerateLoadIndex(builder, leftPHI);
					auto newLeft = MakeLoad(builder, builder->CreateGEP(leftTypeArgs, builder->CreateSub(MakeInt32(-1), typeVarIndex)));
					auto newSubsts = MakeLoad(builder, leftSubstPHI, MakeInt32(TypeArgumentListStackFields::Next));
					leftPHI->addIncoming(newLeft, builder->GetInsertBlock());
					leftSubstPHI->addIncoming(newSubsts, builder->GetInsertBlock());
					builder->CreateBr(leftResolveBlock);
				}

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
				auto namedTypeMatch = builder->CreateICmpEQ(ConstantExpr::getPtrToInt(clsType->Named->GetLLVMElement(*fun->getParent()), numtype(intptr_t)), builder->CreatePtrToInt(ifacePHI, numtype(intptr_t)));
				builder->CreateCondBr(namedTypeMatch, foundNamedTypeMatch, failBlock);

				builder->SetInsertPoint(foundNamedTypeMatch);
				int argIndex = 0;
				BasicBlock* nextCheck = builder->GetInsertBlock();
				for (auto* targ : clsType->Arguments)
				{
					argIndex++; //needs to be at least -1 anyway
					nextCheck = BasicBlock::Create(LLVMCONTEXT, "typeArg" + std::to_string(argIndex), fun);
					auto argLeft = MakeLoad(builder, builder->CreateGEP(typeArgsPHI, MakeInt32(-argIndex)));
					CreateInlineTypeEqCheck(builder, argLeft, targ, leftSubstPHI, rightsubstitutions, nextCheck, failBlock, errorBlock);
					builder->SetInsertPoint(nextCheck);
				}
				builder->CreateBr(successBlock);
			}
			break;
			case TypeKind::TKMaybe:
			{
				BasicBlock* maybeBlock = nullptr;
				RTTypeHead::GenerateTypeKindSwitch(builder, leftType, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &maybeBlock, failBlock);
			}
			default:
				throw new std::exception();
			}
		}


		void RTSubtyping::CreateInlineSubtypingCheck(NomBuilder& builder, llvm::Value* leftTypeWithoutSubstitutions, NomTypeRef rightType, llvm::Value* rightSubstitutions, BasicBlock* successBlock, BasicBlock* failBlock, llvm::BasicBlock* errorBlock)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			switch (rightType->GetKind())
			{
			case TypeKind::TKTop:
			case TypeKind::TKDynamic:
				builder->CreateBr(successBlock);
				break;
			case TypeKind::TKVariable:
			{
				auto rightTypeSubst = MakeLoad(builder, rightSubstitutions, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types), MakeInt32(-(((NomTypeVarRef)rightType)->GetIndex() + 1)) });
				auto newSubsts = MakeLoad(builder, rightSubstitutions, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) });
				Function* subtypingFun = RTSubtyping::Instance().GetLLVMElement(*fun->getParent());
				auto subtypingResult = builder->CreateCall(subtypingFun, { leftTypeWithoutSubstitutions, rightTypeSubst, ConstantPointerNull::get(TypeArgumentListStackType()->getPointerTo()), newSubsts });
				subtypingResult->setCallingConv(NOMCC);
				builder->CreateCondBr(builder->CreateICmpEQ(subtypingResult, MakeUInt(2, 3)), successBlock, failBlock);
			}
			break;
			case TypeKind::TKClass:
			{
				auto clsType = (NomClassTypeRef)rightType;
				BasicBlock* classTypeBlock = nullptr, * instanceTypeBlock = nullptr;

				RTTypeHead::GenerateTypeKindSwitch(builder, leftTypeWithoutSubstitutions, &classTypeBlock, nullptr, nullptr, nullptr, &instanceTypeBlock, nullptr, nullptr, nullptr, nullptr, errorBlock);

				BasicBlock* clsMergeBlock = BasicBlock::Create(LLVMCONTEXT, "classInfoMerge", fun);
				builder->SetInsertPoint(clsMergeBlock);
				auto typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2, "typeArgs");
				auto ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), 2, "interfacePtr");

				if (classTypeBlock != nullptr)
				{
					builder->SetInsertPoint(classTypeBlock);
					typeArgsPHI->addIncoming(RTClassType::GetTypeArgumentsPtr(builder, leftTypeWithoutSubstitutions), builder->GetInsertBlock());
					ifacePHI->addIncoming(RTClassType::GenerateReadClassDescriptorLink(builder, leftTypeWithoutSubstitutions), builder->GetInsertBlock());
					builder->CreateBr(clsMergeBlock);

				}
				if (instanceTypeBlock != nullptr)
				{
					builder->SetInsertPoint(instanceTypeBlock);
					typeArgsPHI->addIncoming(RTInstanceType::GetTypeArgumentsPtr(builder, leftTypeWithoutSubstitutions), builder->GetInsertBlock());
					ifacePHI->addIncoming(RTInstanceType::GenerateReadClassDescriptorLink(builder, leftTypeWithoutSubstitutions), builder->GetInsertBlock());
					builder->CreateBr(clsMergeBlock);
				}

				builder->SetInsertPoint(clsMergeBlock);

				BasicBlock* foundNamedTypeMatch = BasicBlock::Create(LLVMCONTEXT, "foundNamedTypeMatch", fun);
				BasicBlock* foundNamedTypeMatchLoad = BasicBlock::Create(LLVMCONTEXT, "foundNamedTypeMatch$Load", fun);
				BasicBlock* superTypeLoopHead = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Head", fun);
				BasicBlock* superTypeLoopBody = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Body", fun);
				BasicBlock* superTypeLoopBodyEnd = BasicBlock::Create(LLVMCONTEXT, "superTypeLoop$Body$End", fun);
				auto namedTypeMatch = builder->CreateICmpEQ(ConstantExpr::getPtrToInt(clsType->Named->GetLLVMElement(*fun->getParent()), numtype(intptr_t)), builder->CreatePtrToInt(ifacePHI, numtype(intptr_t)));
				builder->CreateCondBr(namedTypeMatch, foundNamedTypeMatch, superTypeLoopHead);

				builder->SetInsertPoint(foundNamedTypeMatch);
				PHINode* foundMatchTypeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2);
				PHINode* foundSubstitutionsPHI = builder->CreatePHI(TypeArgumentListStackType()->getPointerTo(), 2);
				foundMatchTypeArgsPHI->addIncoming(typeArgsPHI, clsMergeBlock);
				foundSubstitutionsPHI->addIncoming(ConstantPointerNull::get(TypeArgumentListStackType()->getPointerTo()), clsMergeBlock);
				int argIndex = 0;
				BasicBlock* nextCheck = builder->GetInsertBlock();
				for (auto* targ : clsType->Arguments)
				{
					argIndex++; //needs to be at least -1 anyway
					nextCheck = BasicBlock::Create(LLVMCONTEXT, "typeArg" + std::to_string(argIndex), fun);
					auto argLeft = MakeLoad(builder, builder->CreateGEP(foundMatchTypeArgsPHI, MakeInt32(-argIndex)));
					CreateInlineTypeEqCheck(builder, argLeft, targ, foundSubstitutionsPHI, rightSubstitutions, nextCheck, failBlock, errorBlock);
					builder->SetInsertPoint(nextCheck);
				}
				builder->CreateBr(successBlock);

				builder->SetInsertPoint(superTypeLoopHead);
				auto superInstances = RTInterface::GenerateReadSuperInstances(builder, ifacePHI);
				auto superInstanceCount = RTInterface::GenerateReadSuperInstanceCount(builder, ifacePHI);
				auto currentIndex = MakeIntLike(superInstanceCount, 0);
				auto areInstancesLeft = builder->CreateICmpULT(currentIndex, superInstanceCount);
				builder->CreateCondBr(areInstancesLeft, superTypeLoopBody, failBlock);

				builder->SetInsertPoint(superTypeLoopBody);
				auto indexPHI = builder->CreatePHI(currentIndex->getType(), 2, "superInstanceIndex");
				indexPHI->addIncoming(currentIndex, superTypeLoopHead);
				auto currentSuperIface = MakeLoad(builder, builder->CreateGEP(superInstances, { indexPHI, MakeInt32(SuperInstanceEntryFields::Class) }));
				namedTypeMatch = builder->CreateICmpEQ(ConstantExpr::getPtrToInt(clsType->Named->GetLLVMElement(*fun->getParent()), numtype(intptr_t)), builder->CreatePtrToInt(currentSuperIface, numtype(intptr_t)));
				builder->CreateCondBr(namedTypeMatch, foundNamedTypeMatchLoad, superTypeLoopBodyEnd);

				builder->SetInsertPoint(foundNamedTypeMatchLoad);
				auto superInstanceTypeArgs = MakeLoad(builder, builder->CreateGEP(superInstances, { indexPHI, MakeInt32(SuperInstanceEntryFields::TypeArgs) }));
				auto tarls = builder->CreateAlloca(TypeArgumentListStackType(), MakeInt32(1));
				MakeStore(builder, typeArgsPHI, tarls, MakeInt32(TypeArgumentListStackFields::Types));
				MakeStore(builder, ConstantPointerNull::get(TypeArgumentListStackType()->getPointerTo()), tarls, MakeInt32(TypeArgumentListStackFields::Next));
				foundMatchTypeArgsPHI->addIncoming(superInstanceTypeArgs, builder->GetInsertBlock());
				foundSubstitutionsPHI->addIncoming(tarls, builder->GetInsertBlock());
				builder->CreateBr(foundNamedTypeMatch);

				builder->SetInsertPoint(superTypeLoopBodyEnd);
				auto newIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1), "newIndex");
				indexPHI->addIncoming(newIndex, superTypeLoopBodyEnd);
				areInstancesLeft = builder->CreateICmpULT(newIndex, superInstanceCount);
				builder->CreateCondBr(areInstancesLeft, superTypeLoopBody, failBlock);
			}
			break;
			case TypeKind::TKMaybe:
			{
				BasicBlock* notNullBlock = BasicBlock::Create(LLVMCONTEXT, "notSubtypeOfNull", fun);
				CreateInlineSubtypingCheck(builder, leftTypeWithoutSubstitutions, NomNullClass::GetInstance()->GetType(), rightSubstitutions, successBlock, notNullBlock, errorBlock);
				builder->SetInsertPoint(notNullBlock);
				CreateInlineSubtypingCheck(builder, leftTypeWithoutSubstitutions, ((NomMaybeTypeRef)rightType)->PotentialType, rightSubstitutions, successBlock, failBlock, errorBlock);
			}
			break;
			default:
				throw new std::exception(); //the other type kinds should not show up here
			}
		}


	}
}