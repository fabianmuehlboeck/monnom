#include "StructuralValueHeader.h"
#include "RefValueHeader.h"
#include "RTCompileConfig.h"
#include "RTSTable.h"
#include "CompileHelpers.h"
#include "Metadata.h"
#include "RTOutput.h"
#include "NomTypeRegistry.h"
#include "RTSubtyping.h"
#include "RTSignature.h"
#include "RTInterface.h"
#include "CompileEnv.h"
#include "RTCast.h"
#include "RTFunctionalInterface.h"
#include "NomClassType.h"
#include "RTLambda.h"
#include "RTStruct.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		static const char* unimplementedMsg = "UNIMPLEMENTED!";
		static const char* notAFunctionalInterfaceMsg = "Trying to cast lambda to non-functional interface!";
		llvm::StructType* StructuralValueHeader::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					arrtype(TYPETYPE, 0),																	//Closure type args
					RefValueHeader::GetLLVMType(),															//vtable
					(NomLambdaOptimizationLevel > 0 ? (llvm::Type*)POINTERTYPE : arrtype(POINTERTYPE, 0)),	//potential space for raw invoke pointer
					numtype(intptr_t) /*RTSTable::GetLLVMType()->getPointerTo()*/,							//structural metadata (with kind flag)
					POINTERTYPE																				//cast data
				);
			}
			return shst;
		}

		void StructuralValueHeader::GenerateInitializationCode(NomBuilder& builder, llvm::Value* sValue, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::Value* vTablePtr, llvm::Value* sTablePtr)
		{
			auto castedDescriptor = builder->CreatePointerCast(sValue, GetLLVMType()->getPointerTo());
			int targIndex = -1;
			for (auto& targ : typeArguments)
			{
				auto targAddress = builder->CreateGEP(castedDescriptor, { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::TypeArgs), MakeInt32(targIndex) });
				MakeInvariantStore(builder, targ, targAddress);
				targIndex--;
			}
			RefValueHeader::GenerateInitializerCode(builder, builder->CreateGEP(castedDescriptor, { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }), vTablePtr);
			MakeInvariantStore(builder, builder->CreatePtrToInt(sTablePtr, numtype(intptr_t)), castedDescriptor, MakeInt32(StructuralValueHeaderFields::STable));
		}

		llvm::Value* StructuralValueHeader::GenerateReadTypeArgsPtr(NomBuilder& builder, llvm::Value* sValue)
		{
			return builder->CreateGEP(builder->CreatePointerCast(sValue, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::TypeArgs) });
		}

		llvm::Value* StructuralValueHeader::GenerateReadCastData(NomBuilder& builder, llvm::Value* sValue)
		{
			auto loadInst = MakeLoad(builder, sValue, GetLLVMType()->getPointerTo(), MakeInt32(StructuralValueHeaderFields::CastData), "castData", AtomicOrdering::Acquire);
			return loadInst;
		}

		llvm::Value* StructuralValueHeader::GenerateReadSTablePtr(NomBuilder& builder, llvm::Value* sValue)
		{
			auto loadInst = MakeLoad(builder, sValue, GetLLVMType()->getPointerTo(), MakeInt32(StructuralValueHeaderFields::STable));
			loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			return builder->CreateIntToPtr(builder->CreateAnd(loadInst, ConstantExpr::getXor(ConstantInt::getAllOnesValue(loadInst->getType()), MakeIntLike(loadInst, 7))), RTSTable::GetLLVMType()->getPointerTo());
		}
		llvm::Value* StructuralValueHeader::GenerateReadRawSTablePtr(NomBuilder& builder, llvm::Value* sValue)
		{
			auto loadInst = MakeLoad(builder, sValue, GetLLVMType()->getPointerTo(), MakeInt32(StructuralValueHeaderFields::STable));
			loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			return builder->CreateIntToPtr(loadInst, RTSTable::GetLLVMType()->getPointerTo());
		}

		llvm::Value* StructuralValueHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* sValue, llvm::Value* index)
		{
			auto targAddress = builder->CreateGEP(builder->CreatePointerCast(sValue, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-1), builder->CreateZExtOrTrunc(index, inttype(32))) });
			auto loadInst = MakeLoad(builder, targAddress, "typeArgument");
			loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			return loadInst;
		}


		llvm::Value* StructuralValueHeader::GenerateWriteCastTypePointerCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* newPtr, llvm::Value* oldPtr)
		{
			auto argPtr = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructuralValueHeaderFields::CastData) });
			return builder->CreateAtomicCmpXchg(argPtr, oldPtr, newPtr, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		}

		int StructuralValueHeader::GenerateStructuralValueKindCase(NomBuilder& builder, llvm::Value* sValue, llvm::Value** sTableVar, llvm::BasicBlock** lambdaBlockVar, llvm::BasicBlock** recordBlockVar, llvm::BasicBlock** partialAppBlockVar, llvm::BasicBlock** errorBlockVar)
		{
			static const char* unhandledErrorMsg = "Unhandled structural value!";
			auto loadInst = MakeInvariantLoad(builder, sValue, GetLLVMType()->getPointerTo(), MakeInt32(StructuralValueHeaderFields::STable));
			loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			auto sTableTag = builder->CreateTrunc(loadInst, inttype(3), "sTableTag");
			if (sTableVar != nullptr)
			{
				if (*sTableVar == nullptr)
				{
					*sTableVar = builder->CreateIntToPtr(builder->CreateXor(loadInst, builder->CreateZExt(sTableTag, loadInst->getType())), RTSTable::GetLLVMType()->getPointerTo());
				}
			}
			BasicBlock* lambdaBlock = nullptr, * recordBlock = nullptr, * partialAppBlock = nullptr, * errorBlock = nullptr;
			int cases = 0;
			if (errorBlockVar != nullptr)
			{
				if (*errorBlockVar != nullptr)
				{
					errorBlock = *errorBlockVar;
				}
				cases++;
			}
			if (lambdaBlockVar != nullptr)
			{
				if (*lambdaBlockVar != nullptr)
				{
					lambdaBlock = *lambdaBlockVar;
				}
				else
				{
					lambdaBlock = BasicBlock::Create(LLVMCONTEXT, "isLambda", builder->GetInsertBlock()->getParent());
					*lambdaBlockVar = lambdaBlock;
				}
				cases++;
			}
			else
			{
				if (errorBlock == nullptr)
				{
					errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unhandledErrorMsg);
				}
				lambdaBlock = errorBlock;
			}
			if (recordBlockVar != nullptr)
			{
				if (*recordBlockVar != nullptr)
				{
					recordBlock = *recordBlockVar;
				}
				else
				{
					recordBlock = BasicBlock::Create(LLVMCONTEXT, "isRecord", builder->GetInsertBlock()->getParent());
					*recordBlockVar = recordBlock;
				}
				cases++;
			}
			else
			{
				if (errorBlock == nullptr)
				{
					errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unhandledErrorMsg);
				}
				recordBlock = errorBlock;
			}
			if (partialAppBlockVar != nullptr)
			{
				if (*partialAppBlockVar != nullptr)
				{
					partialAppBlock = *partialAppBlockVar;
				}
				else
				{
					partialAppBlock = BasicBlock::Create(LLVMCONTEXT, "isPartialApp", builder->GetInsertBlock()->getParent());
					*partialAppBlockVar = partialAppBlock;
				}
				cases++;
			}
			else
			{
				if (errorBlock == nullptr)
				{
					errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unhandledErrorMsg);
				}
				partialAppBlock = errorBlock;
			}

			auto kindSwitch = builder->CreateSwitch(sTableTag, lambdaBlock, 3);
			kindSwitch->addCase(MakeIntLike(sTableTag, (uint64_t)RTDescriptorKind::Lambda), lambdaBlock);
			kindSwitch->addCase(MakeIntLike(sTableTag, (uint64_t)RTDescriptorKind::Struct), recordBlock);
			kindSwitch->addCase(MakeIntLike(sTableTag, (uint64_t)RTDescriptorKind::PartialApplication), partialAppBlock);

			if (errorBlock != nullptr && errorBlockVar != nullptr && *errorBlockVar == nullptr)
			{
				*errorBlockVar = errorBlock;
			}

			return cases;
		}

		void StructuralValueHeader::GenerateMonotonicStructuralCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, NomClassTypeRef rightType, llvm::Value* outerStack)
		{
			const unsigned int extractIndex[2] = { 0,1 };
			auto rightNamed = rightType->Named;
			if (!rightNamed->IsInterface())
			{
				builder->CreateBr(failBlock);
				return;
			}

			auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "monoCastStart", fun);
			BasicBlock* cmpxLoopBlock = BasicBlock::Create(LLVMCONTEXT, "monoCastCMPXLoop$Head", fun);
			BasicBlock* firstCastBlock = BasicBlock::Create(LLVMCONTEXT, "firstCast", fun);
			BasicBlock* notFirstCastBlock = BasicBlock::Create(LLVMCONTEXT, "notFirstCast", fun);
			BasicBlock* unimplementedBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplementedMsg);
			builder->CreateBr(startBlock);

			builder->SetInsertPoint(startBlock);
			auto castData = StructuralValueHeader::GenerateReadCastData(builder, value);
			builder->CreateBr(cmpxLoopBlock);

			builder->SetInsertPoint(cmpxLoopBlock);
			auto currentCastData = builder->CreatePHI(castData->getType(), 2, "castDataPHI");
			currentCastData->addIncoming(castData, startBlock);
			auto hasBeenCast = builder->CreateIsNotNull(currentCastData);
			builder->CreateCondBr(hasBeenCast, notFirstCastBlock, firstCastBlock);

			{
				builder->SetInsertPoint(firstCastBlock);
				Value* sTableVar = nullptr;
				BasicBlock* lambdaBlock = nullptr, * recordBlock = nullptr;

				GenerateStructuralValueKindCase(builder, value, &sTableVar, &lambdaBlock, &recordBlock, &unimplementedBlock, &unimplementedBlock);


				BasicBlock* doWriteCastTypeBlock = BasicBlock::Create(LLVMCONTEXT, "writeSingleCastType", fun);
				BasicBlock* potentiallyCheckSignatureBlock = doWriteCastTypeBlock;
				if (rightNamed->HasLambdaMethod())
				{
					if (RTConfig_CheckLambdaSignaturesAtCast)
					{
						potentiallyCheckSignatureBlock = BasicBlock::Create(LLVMCONTEXT, "checkLambdaSignature", fun);
						BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
						BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);

						builder->SetInsertPoint(potentiallyCheckSignatureBlock);

						auto signature = RTSTable::GenerateReadSignature(builder, sTableVar);
						auto leftSubst = RTSubstStackValue(builder, builder->CreateGEP(StructuralValueHeader::GenerateReadTypeArgsPtr(builder, value), { MakeInt32(0),MakeInt32(0) }));
						auto lambdaMethod = rightNamed->GetLambdaMethod();
						NomInstantiationRef<NomCallable> methodRef = NomInstantiationRef<NomCallable>(lambdaMethod, rightType->Arguments);
						auto sigSubtResult = RTSignature::GenerateInlinedSignatureSubtyping(builder, signature, leftSubst, methodRef, outerStack, false);
						leftSubst.MakeRelease(builder);
						auto resultSwitch = builder->CreateSwitch(sigSubtResult, failBlock);
						resultSwitch->addCase(MakeIntLike(sigSubtResult, 3), pessimisticSignatureMatchBlock);
						resultSwitch->addCase(MakeIntLike(sigSubtResult, 1), optimisticSignatureMatchBlock);

						BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
						BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);
						BasicBlock* adjustRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "adjustRawInvoke", fun);

						builder->SetInsertPoint(pessimisticSignatureMatchBlock);
						auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, signature), numtype(intptr_t)), MakeInt<intptr_t>((intptr_t)lambdaMethod->GetLLVMFunctionType()), "protoMatch");
						builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

						builder->SetInsertPoint(adjustRawInvokeBlock);
						auto rawInvokePHI = builder->CreatePHI(POINTERTYPE, 3, "rawInvokeToWrite");
						{
							auto cmpxresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokePHI);
							auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
							builder->CreateCondBr(updateSuccess, doWriteCastTypeBlock, startBlock);
						}

						builder->SetInsertPoint(prototypeMatchBlock);
						{
							auto rawInvokePtr = RTFunctionalInterface::GenerateReadCheckedRawInvokeWrapper(builder, rightNamed->GetLLVMElement(*fun->getParent()));
							rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
						}
						builder->CreateBr(adjustRawInvokeBlock);

						builder->SetInsertPoint(prototypeMismatchBlock);
						{
							auto rawInvokePtr = RTFunctionalInterface::GenerateReadProtoMismatchRawInvokeWrapper(builder, rightNamed->GetLLVMElement(*fun->getParent()));
							rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
						}
						builder->CreateBr(adjustRawInvokeBlock);

						builder->SetInsertPoint(optimisticSignatureMatchBlock);
						{
							auto rawInvokePtr = RTFunctionalInterface::GenerateReadCastingRawInvokeWrapper(builder, rightNamed->GetLLVMElement(*fun->getParent()));
							rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
						}
						builder->CreateBr(adjustRawInvokeBlock);
					}
					else
					{
						throw new std::exception(); //todo: implement not checking lambda signatures (just write pointer to wrapper)
					}
				}

				if (rightNamed->IsFunctional() || rightNamed->HasNoMethods())
				{
					builder->SetInsertPoint(lambdaBlock);
					builder->CreateBr(potentiallyCheckSignatureBlock);
				}
				else
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, notAFunctionalInterfaceMsg, lambdaBlock);
				}

				{
					builder->SetInsertPoint(recordBlock);
					builder->CreateBr(potentiallyCheckSignatureBlock);
				}

				{
					BasicBlock* cmpxchgFailBlock = BasicBlock::Create(LLVMCONTEXT, "cmpxchgfail", fun);
					builder->SetInsertPoint(doWriteCastTypeBlock);
					Value* castWriteType = nullptr;
					if (rightType->ContainsVariables())
					{
						if (outerStack == ConstantPointerNull::get(RTSubstStack::GetLLVMType()->getPointerTo()))
						{
							throw new std::exception();
						}
						auto uniquedType = builder->CreateCall(typeUniqueFun, { rightType->GetLLVMElement(*fun->getParent()), outerStack });
						uniquedType->setCallingConv(typeUniqueFun->getCallingConv());
						castWriteType = uniquedType;
					}
					else
					{
						castWriteType = rightType->GetLLVMElement(*fun->getParent());
					}
					auto cmpxresult = StructuralValueHeader::GenerateWriteCastTypePointerCMPXCHG(builder, value, builder->CreatePointerCast(castWriteType, POINTERTYPE), currentCastData);
					auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(updateSuccess, successBlock, cmpxchgFailBlock);

					builder->SetInsertPoint(cmpxchgFailBlock);
					auto lambdaCastTypeOnFirstFail = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex, 1));
					currentCastData->addIncoming(lambdaCastTypeOnFirstFail, builder->GetInsertBlock());
					builder->CreateBr(cmpxLoopBlock);
				}
			}

			{
				BasicBlock* singleCastBlock = BasicBlock::Create(LLVMCONTEXT, "checkSinglyCastSubtyping", fun);

				builder->SetInsertPoint(notFirstCastBlock);
				Value* castTag = builder->CreateTrunc(builder->CreatePtrToInt(currentCastData, numtype(intptr_t)), inttype(1));
				builder->CreateCondBr(castTag, unimplementedBlock, singleCastBlock);

				builder->SetInsertPoint(singleCastBlock);
				RTSubtyping::CreateInlineSubtypingCheck(builder, builder->CreatePointerCast(currentCastData, TYPETYPE), nullptr, rightType, outerStack, successBlock, nullptr, unimplementedBlock);
			}
		}

		void StructuralValueHeader::GenerateMonotonicStructuralCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* rightType, llvm::Value* rightIface, llvm::Value* rightTypeArgs, llvm::Value* outerStack)
		{
			unsigned int extractIndex[2];
			extractIndex[0] = 0;
			extractIndex[1] = 1;

			auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
			auto signatureSubtyping = RTSignature::Instance().GetLLVMElement(*fun->getParent());
			auto subtypingFun = RTSubtyping::Instance().GetLLVMElement(*fun->getParent());

			BasicBlock* cmpxchgFailBlock = BasicBlock::Create(LLVMCONTEXT, "cmpxchgfail", fun);
			BasicBlock* notFunctionalInterface = RTOutput_Fail::GenerateFailOutputBlock(builder, notAFunctionalInterfaceMsg);
			BasicBlock* unimplementedBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplementedMsg);

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "monoCastStart", fun);
			builder->CreateBr(startBlock);

			builder->SetInsertPoint(startBlock);
			auto currentCastData = StructuralValueHeader::GenerateReadCastData(builder, value);
			auto hasBeenCast = builder->CreateIsNotNull(currentCastData);
			Value* lambdaCastTypeOnFirstFail = nullptr;

			BasicBlock* firstCastBlock = BasicBlock::Create(LLVMCONTEXT, "LambdaCast$firstCast", fun);
			BasicBlock* multiCastBlock = BasicBlock::Create(LLVMCONTEXT, "LambdaCast$multiCast", fun);

			BasicBlock* incomingBlock = builder->GetInsertBlock();
			builder->CreateCondBr(hasBeenCast, multiCastBlock, firstCastBlock);
			{

				BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
				BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
				BasicBlock* tryWriteTypeCastBlock = BasicBlock::Create(LLVMCONTEXT, "trycmpxchg", fun);
				BasicBlock* checkSignatureRequiredBlock = BasicBlock::Create(LLVMCONTEXT, "checkSignatureRequired", fun);
				BasicBlock* checkSignatureExistsBlock = BasicBlock::Create(LLVMCONTEXT, "checkSignatureExists", fun);
				BasicBlock* checkSignatureBlock = BasicBlock::Create(LLVMCONTEXT, "checkSignature", fun);

				builder->SetInsertPoint(firstCastBlock);

				BasicBlock* lambdaBlock = nullptr, * recordBlock = nullptr, * partialAppBlock = nullptr;
				Value* sTableVar = nullptr;

				auto interfaceSig = RTInterface::GenerateReadSignature(builder, rightIface);

				GenerateStructuralValueKindCase(builder, value, &sTableVar, &lambdaBlock, &recordBlock, &unimplementedBlock, &unimplementedBlock);

				if (lambdaBlock != nullptr)
				{
					builder->SetInsertPoint(lambdaBlock);
					auto interfaceFlag = RTInterface::GenerateReadFlags(builder, rightIface);
					auto flagTarget = MakeIntLike(interfaceFlag, (long)(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
					builder->CreateCondBr(builder->CreateICmpEQ(builder->CreateAnd(interfaceFlag, flagTarget), flagTarget), checkSignatureExistsBlock, notFunctionalInterface);
				}

				if (recordBlock != nullptr)
				{
					builder->SetInsertPoint(recordBlock);
					builder->CreateBr(checkSignatureRequiredBlock);
				}

				builder->SetInsertPoint(checkSignatureRequiredBlock);
				builder->CreateCondBr(builder->CreateIsNull(interfaceSig), tryWriteTypeCastBlock, checkSignatureExistsBlock);

				builder->SetInsertPoint(checkSignatureExistsBlock);
				auto structuralValueSig = RTSTable::GenerateReadSignature(builder, sTableVar);
				builder->CreateCondBr(builder->CreateIsNull(structuralValueSig), failBlock, checkSignatureBlock);

				builder->SetInsertPoint(checkSignatureBlock);

				auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
				MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
				MakeStore(builder, rightTypeArgs, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

				auto signatureMatch = builder->CreateCall(signatureSubtyping, { structuralValueSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
				signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
				auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

				BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
				BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);
				BasicBlock* adjustRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "adjustRawInvoke", fun);

				builder->SetInsertPoint(pessimisticSignatureMatchBlock);
				auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, structuralValueSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
				builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

				builder->SetInsertPoint(adjustRawInvokeBlock);
				auto rawInvokePHI = builder->CreatePHI(POINTERTYPE, 3, "rawInvokeToWrite");
				{
					auto cmpxresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokePHI);
					auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(updateSuccess, tryWriteTypeCastBlock, startBlock);
				}

				builder->SetInsertPoint(prototypeMatchBlock);
				{
					auto rawInvokePtr = RTFunctionalInterface::GenerateReadCheckedRawInvokeWrapper(builder, rightIface);
					rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
				}
				builder->CreateBr(adjustRawInvokeBlock);

				builder->SetInsertPoint(prototypeMismatchBlock);
				{
					auto rawInvokePtr = RTFunctionalInterface::GenerateReadProtoMismatchRawInvokeWrapper(builder, rightIface);
					rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
				}
				builder->CreateBr(adjustRawInvokeBlock);

				builder->SetInsertPoint(optimisticSignatureMatchBlock);
				{
					auto rawInvokePtr = RTFunctionalInterface::GenerateReadCastingRawInvokeWrapper(builder, rightIface);
					rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
				}
				builder->CreateBr(adjustRawInvokeBlock);

				builder->SetInsertPoint(tryWriteTypeCastBlock);
				auto uniquedType = builder->CreateCall(typeUniqueFun, { rightType, outerStack });
				uniquedType->setCallingConv(typeUniqueFun->getCallingConv());

				auto cmpxresult = StructuralValueHeader::GenerateWriteCastTypePointerCMPXCHG(builder, value, builder->CreatePointerCast(uniquedType, POINTERTYPE), currentCastData);
				auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
				builder->CreateCondBr(updateSuccess, successBlock, cmpxchgFailBlock);

				builder->SetInsertPoint(cmpxchgFailBlock);
				lambdaCastTypeOnFirstFail = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex, 1));
				builder->CreateBr(multiCastBlock);
			}

			{
				builder->SetInsertPoint(multiCastBlock);
				auto lambdaCastTypePHI = builder->CreatePHI(currentCastData->getType(), 2);
				lambdaCastTypePHI->addIncoming(currentCastData, incomingBlock);
				lambdaCastTypePHI->addIncoming(lambdaCastTypeOnFirstFail, cmpxchgFailBlock);

				BasicBlock* singleCastEntryBlock = BasicBlock::Create(LLVMCONTEXT, "singleCastEntry", fun);
				BasicBlock* multiCastListBlock = BasicBlock::Create(LLVMCONTEXT, "multiCastList", fun);

				auto lambdaCastTag = builder->CreateTrunc(builder->CreatePtrToInt(lambdaCastTypePHI, numtype(intptr_t)), inttype(1));
				builder->CreateCondBr(lambdaCastTag, multiCastListBlock, singleCastEntryBlock);

				builder->SetInsertPoint(singleCastEntryBlock);
				auto cleanLeftType = builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(lambdaCastTypePHI, numtype(intptr_t)), ConstantExpr::getXor(llvm::ConstantInt::getAllOnesValue(numtype(intptr_t)), MakeInt<intptr_t>(7))), TYPETYPE);
				auto subtypingCall = builder->CreateCall(subtypingFun, { cleanLeftType, rightType, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), outerStack });
				subtypingCall->setCallingConv(subtypingFun->getCallingConv());
				builder->CreateCondBr(builder->CreateICmpEQ(subtypingCall, MakeIntLike(subtypingCall, 3)), successBlock, unimplementedBlock);

				builder->SetInsertPoint(multiCastListBlock);
				builder->CreateBr(unimplementedBlock);
			}
		}
	}
}