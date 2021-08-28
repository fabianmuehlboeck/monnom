#include "RTCast.h"
#include "NomDynamicType.h"
#include "CompileHelpers.h"
#include "NomTopType.h"
#include "NomClassType.h"
#include "NomDynamicType.h"
#include "NomBottomType.h"
#include "BoolClass.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "RTTypeEq.h"
#include "NomTypeVar.h"
#include "llvm/ADT/SmallVector.h"
#include "RTOutput.h"
#include "RTClassType.h"
#include "ObjectClass.h"
#include "RTConfig.h"
#include "CastStats.h"
#include "LambdaHeader.h"
#include "RTLambda.h"
#include "RTFunctionalInterface.h"
#include "RTLambdaInterface.h"
#include "NomTypeRegistry.h"
#include "RTInterface.h"
#include "RTSubtyping.h"
#include "RTSignature.h"
#include "RTStruct.h"
#include "RTGeneralInterface.h"
#include "RTStructInterface.h"
#include "StructHeader.h"
#include "NomType.h"
#include "CallingConvConf.h"
#include "NullClass.h"
#include "NomMaybeType.h"
#include "RTInstanceType.h"
#include "RTMaybeType.h"
#include "RTCompileConfig.h"
#include "RTDictionary.h"
#include "StructuralValueHeader.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		//void GenerateMonotonicLambdaCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* sTable, llvm::Value* rightType, llvm::Value* rightIface, llvm::Value* rightTypeArgs, llvm::Value* outerStack, bool guaranteedNoTypeArgs)
		//{
		//	BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "monoLambdaCastStart", fun);
		//	builder->CreateBr(startBlock);
		//	builder->SetInsertPoint(startBlock);
		//	unsigned int extractIndex[2];
		//	extractIndex[0] = 0;
		//	extractIndex[1] = 1;
		//	auto lambdaCastType = LambdaHeader::GenerateReadCastTypePointer(builder, value);
		//	auto hasBeenCast = builder->CreateIsNotNull(lambdaCastType);
		//	auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
		//	auto signatureSubtyping = RTSignature::Instance().GetLLVMElement(*fun->getParent());
		//	auto subtypingFun = RTSubtyping::Instance().GetLLVMElement(*fun->getParent());
		//	Value* lambdaCastTypeOnFirstFail = nullptr;

		//	BasicBlock* firstCastBlock = BasicBlock::Create(LLVMCONTEXT, "LambdaCast$firstCast", fun);
		//	BasicBlock* multiCastBlock = BasicBlock::Create(LLVMCONTEXT, "LambdaCast$multiCast", fun);
		//	BasicBlock* cmpxchgFailBlock = BasicBlock::Create(LLVMCONTEXT, "cmpxchgfail", fun);
		//	static const char* notAFunctionalInterfaceMsg = "Trying to cast lambda to non-functional interface!";
		//	BasicBlock* notFunctionalInterface = RTOutput_Fail::GenerateFailOutputBlock(builder, notAFunctionalInterfaceMsg);
		//	static const char* unimplementedMsg = "UNIMPLEMENTED!";
		//	BasicBlock* unimplementedBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, unimplementedMsg);
		//	BasicBlock* incomingBlock = builder->GetInsertBlock();
		//	builder->CreateCondBr(hasBeenCast, multiCastBlock, firstCastBlock);
		//	{
		//		builder->SetInsertPoint(firstCastBlock);
		//		BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
		//		BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
		//		BasicBlock* tryWriteTypeCastBlock = BasicBlock::Create(LLVMCONTEXT, "trycmpxchg", fun);
		//		BasicBlock* checkSignatureBlock = BasicBlock::Create(LLVMCONTEXT, "checkSignature", fun);

		//		auto interfaceFlag = RTInterface::GenerateReadFlags(builder, rightIface);
		//		auto flagTarget = MakeIntLike(interfaceFlag, (long)(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
		//		builder->CreateCondBr(builder->CreateICmpEQ(builder->CreateAnd(interfaceFlag, flagTarget), flagTarget), checkSignatureBlock, notFunctionalInterface);

		//		builder->SetInsertPoint(checkSignatureBlock);
		//		auto uniquedType = builder->CreateCall(typeUniqueFun, { rightType, outerStack });
		//		uniquedType->setCallingConv(typeUniqueFun->getCallingConv());

		//		auto lambdaSig = RTLambda::GenerateReadSignature(builder, sTable);
		//		auto interfaceSig = RTFunctionalInterface::GenerateReadRTSignature(builder, rightIface);

		//		auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
		//		MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
		//		MakeStore(builder, rightTypeArgs, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

		//		auto signatureMatch = builder->CreateCall(signatureSubtyping, { lambdaSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
		//		signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
		//		auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
		//		signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
		//		signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

		//		BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
		//		BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);
		//		BasicBlock* adjustRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "adjustRawInvoke", fun);

		//		builder->SetInsertPoint(pessimisticSignatureMatchBlock);
		//		auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, lambdaSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
		//		builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

		//		builder->SetInsertPoint(adjustRawInvokeBlock);
		//		auto rawInvokePHI = builder->CreatePHI(POINTERTYPE, 3, "rawInvokeToWrite");
		//		{
		//			auto cmpxresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokePHI);
		//			auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		//			builder->CreateCondBr(updateSuccess, tryWriteTypeCastBlock, startBlock);
		//		}

		//		builder->SetInsertPoint(prototypeMatchBlock);
		//		{
		//			auto rawInvokePtr = RTFunctionalInterface::GenerateReadCheckedRawInvokeWrapper(builder, rightIface);
		//			rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
		//		}
		//		builder->CreateBr(adjustRawInvokeBlock);

		//		builder->SetInsertPoint(prototypeMismatchBlock);
		//		{
		//			auto rawInvokePtr = RTFunctionalInterface::GenerateReadProtoMismatchRawInvokeWrapper(builder, rightIface);
		//			rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
		//		}
		//		builder->CreateBr(adjustRawInvokeBlock);

		//		builder->SetInsertPoint(optimisticSignatureMatchBlock);
		//		{
		//			auto rawInvokePtr = RTFunctionalInterface::GenerateReadCastingRawInvokeWrapper(builder, rightIface);
		//			rawInvokePHI->addIncoming(rawInvokePtr, builder->GetInsertBlock());
		//		}
		//		builder->CreateBr(adjustRawInvokeBlock);

		//		builder->SetInsertPoint(tryWriteTypeCastBlock);
		//		//auto pessimisticMatchPHI = builder->CreatePHI(inttype(2), 2, "pessimisticMatchFlag");
		//		//pessimisticMatchPHI->addIncoming(MakeUInt(2, 2), pessimisticSignatureMatchBlock);
		//		//pessimisticMatchPHI->addIncoming(MakeUInt(2, 0), optimisticSignatureMatchBlock);
		//		//auto writeValue = builder->CreateIntToPtr(builder->CreateOr(builder->CreateZExt(pessimisticMatchPHI, numtype(intptr_t)), builder->CreatePtrToInt(uniquedType, numtype(intptr_t))), POINTERTYPE);
		//		auto cmpxresult = LambdaHeader::GenerateWriteCastTypePointerCMPXCHG(builder, value, builder->CreatePointerCast(uniquedType, POINTERTYPE), lambdaCastType);
		//		auto updateSuccess = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		//		builder->CreateCondBr(updateSuccess, successBlock, cmpxchgFailBlock);

		//		builder->SetInsertPoint(cmpxchgFailBlock);
		//		lambdaCastTypeOnFirstFail = builder->CreateExtractValue(cmpxresult, ArrayRef<unsigned int>(extractIndex, 1));
		//		builder->CreateBr(multiCastBlock);
		//	}

		//	{
		//		builder->SetInsertPoint(multiCastBlock);
		//		auto lambdaCastTypePHI = builder->CreatePHI(lambdaCastType->getType(), 2);
		//		lambdaCastTypePHI->addIncoming(lambdaCastType, incomingBlock);
		//		lambdaCastTypePHI->addIncoming(lambdaCastTypeOnFirstFail, cmpxchgFailBlock);

		//		BasicBlock* singleCastEntryBlock = BasicBlock::Create(LLVMCONTEXT, "singleCastEntry", fun);
		//		BasicBlock* multiCastListBlock = BasicBlock::Create(LLVMCONTEXT, "multiCastList", fun);

		//		auto lambdaCastTag = builder->CreateTrunc(builder->CreatePtrToInt(lambdaCastTypePHI, numtype(intptr_t)), inttype(1));
		//		builder->CreateCondBr(lambdaCastTag, multiCastListBlock, singleCastEntryBlock);

		//		builder->SetInsertPoint(singleCastEntryBlock);
		//		auto cleanLeftType = builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(lambdaCastTypePHI, numtype(intptr_t)), ConstantExpr::getXor(llvm::ConstantInt::getAllOnesValue(numtype(intptr_t)), MakeInt<intptr_t>(7))), TYPETYPE);
		//		auto subtypingCall = builder->CreateCall(subtypingFun, { cleanLeftType, rightType, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), outerStack });
		//		subtypingCall->setCallingConv(subtypingFun->getCallingConv());
		//		builder->CreateCondBr(builder->CreateICmpEQ(subtypingCall, MakeIntLike(subtypingCall,3)), successBlock, unimplementedBlock);

		//		builder->SetInsertPoint(multiCastListBlock);
		//		builder->CreateBr(unimplementedBlock);
		//	}
		//}
		////void GenerateMonotonicLambdaCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* sTable, llvm::Value* rtfunctionalInterface, llvm::Value* outerStack, llvm::Value* clsTypeArgCount, llvm::Value* typeArgsPtr, bool useCastSiteID, bool guaranteedNoTypeArgs)
		////{
		////	unsigned int extractIndex[2];
		////	extractIndex[0] = 0;
		////	extractIndex[1] = 1;
		////	auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
		////	BasicBlock* multiCastBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "multicasting structures not implemented!"); //BasicBlock::Create(LLVMCONTEXT, "multiCastCheck", fun);
		////	llvm::Value* specializedVtable = nullptr;
		////	llvm::Value* rtlambda = LambdaHeader::GenerateReadLambdaMetadata(builder, value);
		////	auto signatureSubtyping = RTSignature::Instance().GetLLVMElement(*fun->getParent());
		////	if (!guaranteedNoTypeArgs)
		////	{
		////		BasicBlock* checkSpecializedVTableBlock = nullptr;
		////		BasicBlock* checkFreeSlotsBlock = nullptr;

		////		BasicBlock* checkNoTypeArgsBlock = builder->GetInsertBlock();
		////		BasicBlock* copyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "copyVTable", fun);

		////		BasicBlock** firstRegularBlock = PreferTypeArgumentsInVTables ? &checkSpecializedVTableBlock : &checkFreeSlotsBlock;
		////		BasicBlock** secondRegularBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &checkSpecializedVTableBlock;

		////		BasicBlock** checkSpecializeVTableFailBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &copyVTableBlock;
		////		BasicBlock** checkFreeSlotsFailBlock = PreferTypeArgumentsInVTables ? &copyVTableBlock : &checkSpecializedVTableBlock;

		////		*firstRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkSpecializedVTable" : "checkFreeSlots", fun);
		////		*secondRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkFreeSlots" : "checkSpecializedVTable", fun);

		////		BasicBlock* checkTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgs", fun);
		////		BasicBlock* checkTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgsLoop", fun);

		////		BasicBlock* castSiteMatch = BasicBlock::Create(LLVMCONTEXT, "castSiteMatch", fun);
		////		BasicBlock* writeOptimizedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeOptimizedVTable", fun);

		////		specializedVtable = RTLambda::GenerateReadSpecializedVTable(builder, rtlambda);
		////		llvm::ConstantInt* castID = nullptr;
		////		if (useCastSiteID)
		////		{
		////			checkNoTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkNoTypeArgs", fun);
		////			auto specializedCastID = RTLambda::GenerateReadSpecializedVTableCastID(builder, rtlambda);
		////			castID = MakeInt(RTCast::nextCastSiteID());
		////			auto specializedTypeMatch = builder->CreateICmpEQ(specializedCastID, castID, "castSiteIsMatch");

		////			builder->CreateCondBr(specializedTypeMatch, castSiteMatch, checkNoTypeArgsBlock);
		////		}

		////		builder->SetInsertPoint(checkNoTypeArgsBlock);
		////		auto noTypeArgs = builder->CreateICmpEQ(clsTypeArgCount, MakeIntLike(clsTypeArgCount, 0), "typeArgCountIsZero");
		////		builder->CreateCondBr(noTypeArgs, writeOptimizedVTableBlock, *firstRegularBlock);

		////		//check specialized vtable
		////		{
		////			builder->SetInsertPoint(checkSpecializedVTableBlock);
		////			BasicBlock* specializedVTableExistsBlock = BasicBlock::Create(LLVMCONTEXT, "specializedVTableExists", fun);

		////			builder->CreateCondBr(builder->CreateIsNotNull(specializedVtable), specializedVTableExistsBlock, *checkSpecializeVTableFailBlock);

		////			builder->SetInsertPoint(specializedVTableExistsBlock);
		////			auto origInterface = RTStructInterface::GetOriginalInterface(builder, specializedVtable);
		////			auto interfacesMatch = CreatePointerEq(builder, origInterface, rtfunctionalInterface);
		////			auto negTypeArgCount = builder->CreateNeg(clsTypeArgCount);
		////			auto castTypeSubstsTypes = MakeLoad(builder, outerStack, MakeInt32(TypeArgumentListStackFields::Types));
		////			auto pointerEqCheckBlock = builder->GetInsertBlock();
		////			builder->CreateCondBr(interfacesMatch, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

		////			builder->SetInsertPoint(checkTypeArgsBlock);
		////			PHINode* argPosPHI = builder->CreatePHI(inttype(32), 2, "typeArgPos");
		////			argPosPHI->addIncoming(negTypeArgCount, pointerEqCheckBlock);
		////			auto typeArgsFinished = builder->CreateICmpEQ(argPosPHI, MakeInt32(0), "typeArgsFinished");
		////			builder->CreateCondBr(typeArgsFinished, checkTypeArgsLoopBlock, castSiteMatch);

		////			builder->SetInsertPoint(checkTypeArgsLoopBlock);
		////			auto typeEqFun = RTTypeEq::Instance().GetLLVMElement(*fun->getParent());
		////			auto leftTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, argPosPHI), "leftTypeArg");
		////			auto rightTypeArg = RTLambdaInterface::GetTypeArgumentNegativeIndex(builder, specializedVtable, argPosPHI);
		////			auto typeEqResult = builder->CreateCall(typeEqFun, { leftTypeArg, castTypeSubstsTypes, rightTypeArg });
		////			typeEqResult->setCallingConv(typeEqFun->getCallingConv());
		////			argPosPHI->addIncoming(builder->CreateAdd(argPosPHI, MakeInt32(1)), builder->GetInsertBlock());
		////			builder->CreateCondBr(typeEqResult, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);
		////		}

		////		//check free slots (at this point we know that targcount>0)
		////		{
		////			builder->SetInsertPoint(checkFreeSlotsBlock);
		////			BasicBlock* enoughFreeSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "enoughFreeTypeArgSlots", fun);
		////			BasicBlock* copyOtherTypeArgumentsLoopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Head", fun);
		////			BasicBlock* copyOtherTypeArgumentsLoopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Body", fun);
		////			auto freeSlots = RefValueHeader::GenerateGetReserveTypeArgsFromVTablePointer(builder, vtable);
		////			auto enoughFreeSlots = builder->CreateICmpULE(clsTypeArgCount, freeSlots, "enoughFreeSlots");
		////			builder->CreateCondBr(enoughFreeSlots, enoughFreeSlotsBlock, *checkFreeSlotsFailBlock);

		////			builder->SetInsertPoint(enoughFreeSlotsBlock);
		////			if (NomCastStats)
		////			{
		////				builder->CreateCall(GetIncEnoughSpaceCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
		////			}
		////			auto firstIndex = MakeInt32(-1);
		////			auto firstTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, firstIndex));
		////			auto firstUniquedTarg = builder->CreateCall(typeUniqueFun, { firstTarg, outerStack });
		////			firstUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			auto cmpxchgresult = LambdaHeader::GenerateWriteCastTypeArgumentCMPXCHG(builder, value, MakeInt32(0), firstUniquedTarg);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, copyOtherTypeArgumentsLoopHeadBlock, multiCastBlock);

		////			builder->SetInsertPoint(copyOtherTypeArgumentsLoopHeadBlock);
		////			auto moreThanOneTypeArg = builder->CreateICmpUGT(clsTypeArgCount, MakeIntLike(clsTypeArgCount, 1));
		////			builder->CreateCondBr(moreThanOneTypeArg, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);

		////			builder->SetInsertPoint(copyOtherTypeArgumentsLoopBodyBlock);
		////			auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
		////			targposPHI->addIncoming(MakeInt32(1), copyOtherTypeArgumentsLoopHeadBlock);
		////			auto nextTargPos = builder->CreateAdd(targposPHI, MakeInt32(1));
		////			targposPHI->addIncoming(nextTargPos, copyOtherTypeArgumentsLoopBodyBlock);

		////			auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, builder->CreateSub(MakeInt32(-1), targposPHI)));
		////			auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, outerStack });
		////			currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			LambdaHeader::GenerateWriteCastTypeArgument(builder, value, targposPHI, currentUniquedTarg); //no cmpxchgs necessary here, as the first one serves as lock

		////			auto hasMoreTypeArgs = builder->CreateICmpUGT(clsTypeArgCount, nextTargPos);
		////			builder->CreateCondBr(hasMoreTypeArgs, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);
		////		}


		////		//copy vtable
		////		{
		////			BasicBlock* doCopyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "doCopyVTableBlock", fun);
		////			builder->SetInsertPoint(doCopyVTableBlock);
		////			PHINode* vtablePHI = builder->CreatePHI(RTLambdaInterface::GetLLVMType()->getPointerTo(), 3);
		////			//block to be continued further below

		////			builder->SetInsertPoint(copyVTableBlock);
		////			{
		////				BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
		////				BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
		////				BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
		////				BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);

		////				auto lambdaSig = RTLambda::GenerateReadSignature(builder, rtlambda);
		////				auto interfaceSig = RTFunctionalInterface::GenerateReadRTSignature(builder, rtfunctionalInterface);

		////				auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
		////				MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
		////				MakeStore(builder, typeArgsPtr, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

		////				auto signatureMatch = builder->CreateCall(signatureSubtyping, { lambdaSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
		////				signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
		////				auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
		////				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
		////				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

		////				builder->SetInsertPoint(pessimisticSignatureMatchBlock);
		////				if (NomCastStats)
		////				{
		////					builder->CreateCall(GetIncCheckedMonoCastsFunction(*fun->getParent()), {});
		////				}
		////				auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, lambdaSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
		////				builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

		////				builder->SetInsertPoint(prototypeMatchBlock);
		////				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMatchVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////				builder->CreateBr(doCopyVTableBlock);

		////				builder->SetInsertPoint(prototypeMismatchBlock);
		////				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMismatchVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////				builder->CreateBr(doCopyVTableBlock);

		////				builder->SetInsertPoint(optimisticSignatureMatchBlock);
		////				if (NomCastStats)
		////				{
		////					builder->CreateCall(GetIncCastingMonoCastsFunction(*fun->getParent()), {});
		////				}
		////				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadCastingVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////				builder->CreateBr(doCopyVTableBlock);

		////			}

		////			BasicBlock* copyTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "copyTypeArgsToVtable", fun);
		////			BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCopiedVTablePtr", fun);
		////			BasicBlock* writeRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "writeRawInvoke", fun);
		////			BasicBlock* adjustStructDataBlock = BasicBlock::Create(LLVMCONTEXT, "adjustStructData", fun);
		////			BasicBlock* tryWriteSpecializedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "tryWriteSpecializedVTable", fun);
		////			BasicBlock* checkEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "checkEnoughPreallocatedSlots", fun);
		////			BasicBlock* notEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "notEnoughPreallocatedSlots", fun);

		////			builder->SetInsertPoint(doCopyVTableBlock);
		////			auto newVtable = RTLambdaInterface::CreateCopyVTable(builder, vtablePHI, clsTypeArgCount);
		////			auto typeArgsTarget = RTLambdaInterface::GetTypeArgumentsPointer(builder, newVtable);
		////			copyVTableBlock = builder->GetInsertBlock();
		////			builder->CreateBr(copyTypeArgsBlock);

		////			builder->SetInsertPoint(copyTypeArgsBlock);
		////			auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
		////			targposPHI->addIncoming(MakeInt32(-1), copyVTableBlock);

		////			auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, targposPHI));
		////			auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, outerStack });
		////			currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			MakeStore(builder, currentUniquedTarg, builder->CreatePointerCast(newVtable, RTLambdaInterface::GetLLVMType()->getPointerTo()), { MakeInt32(RTLambdaInterfaceFields::TypeArgs), targposPHI });

		////			auto nextTargPos = builder->CreateSub(targposPHI, MakeInt32(1));
		////			targposPHI->addIncoming(nextTargPos, builder->GetInsertBlock());
		////			auto targsDone = builder->CreateICmpEQ(targposPHI, builder->CreateNeg(clsTypeArgCount));
		////			builder->CreateCondBr(targsDone, writeRawInvokeBlock, copyTypeArgsBlock);

		////			builder->SetInsertPoint(writeRawInvokeBlock);
		////			auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, newVtable);
		////			auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
		////			auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

		////			builder->SetInsertPoint(writeVTableBlock);
		////			auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, newVtable, vtable);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, adjustStructDataBlock, multiCastBlock);

		////			builder->SetInsertPoint(adjustStructDataBlock);
		////			auto slotsAddress = builder->CreateGEP(builder->CreatePointerCast(rtlambda, RTLambda::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaFields::PreallocatedSlots) });
		////			auto typeArgSlotsCount = MakeLoad(builder, slotsAddress);
		////			auto extTargCount = builder->CreateZExtOrTrunc(clsTypeArgCount, typeArgSlotsCount->getType());
		////			auto specializedVtableIsNull = builder->CreateIsNull(specializedVtable, "specializedVTableIsNull");
		////			builder->CreateCondBr(specializedVtableIsNull, tryWriteSpecializedVTableBlock, checkEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(tryWriteSpecializedVTableBlock);
		////			RTLambda::GenerateWriteSpecializedVTable(builder, rtlambda, castID, newVtable);
		////			tryWriteSpecializedVTableBlock = builder->GetInsertBlock();
		////			builder->CreateBr(checkEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(checkEnoughPreallocatedSlotsBlock);
		////			auto currentPreAllocatedSlotsCount = builder->CreatePHI(typeArgSlotsCount->getType(), 3);
		////			currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, tryWriteSpecializedVTableBlock);
		////			currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, adjustStructDataBlock);
		////			auto enoughPreallocatedSlots = builder->CreateICmpULE(extTargCount, currentPreAllocatedSlotsCount, "enoughPreallocatedSlots");
		////			builder->CreateCondBr(enoughPreallocatedSlots, successBlock, notEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(notEnoughPreallocatedSlotsBlock);
		////			auto updatePreallocatedSlotsResult = builder->CreateAtomicCmpXchg(slotsAddress, typeArgSlotsCount, extTargCount, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		////			currentPreAllocatedSlotsCount->addIncoming(builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex, 1)), builder->GetInsertBlock());
		////			auto updateSuccess = builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(updateSuccess, successBlock, checkEnoughPreallocatedSlotsBlock);
		////		}

		////		// cast site match
		////		{
		////			BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCastSiteMatchVtable", fun);
		////			builder->SetInsertPoint(castSiteMatch);
		////			specializedVtable = RTLambda::GenerateReadSpecializedVTable(builder, rtlambda);

		////			auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, specializedVtable);
		////			auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
		////			auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

		////			builder->SetInsertPoint(writeVTableBlock);
		////			auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
		////		}

		////		builder->SetInsertPoint(writeOptimizedVTableBlock);
		////	}

		////	{
		////		BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
		////		BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
		////		BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
		////		BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);
		////		BasicBlock* writeRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "writeRawInvoke", fun);
		////		BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeVTable", fun);

		////		auto lambdaSig = RTLambda::GenerateReadSignature(builder, rtlambda);
		////		auto interfaceSig = RTFunctionalInterface::GenerateReadRTSignature(builder, rtfunctionalInterface);

		////		auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
		////		MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
		////		MakeStore(builder, typeArgsPtr, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

		////		auto signatureMatch = builder->CreateCall(signatureSubtyping, { lambdaSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
		////		signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
		////		auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
		////		signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
		////		signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

		////		builder->SetInsertPoint(writeRawInvokeBlock);
		////		auto vtablePHI = builder->CreatePHI(RTLambdaInterface::GetLLVMType()->getPointerTo(), 3);
		////		auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, vtablePHI);
		////		auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
		////		auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////		builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

		////		builder->SetInsertPoint(pessimisticSignatureMatchBlock);
		////		if (NomCastStats)
		////		{
		////			builder->CreateCall(GetIncCheckedMonoCastsFunction(*fun->getParent()), {});
		////		}
		////		auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, lambdaSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
		////		builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

		////		builder->SetInsertPoint(prototypeMatchBlock);
		////		vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMatchVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////		builder->CreateBr(writeRawInvokeBlock);

		////		builder->SetInsertPoint(prototypeMismatchBlock);
		////		vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMismatchVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////		builder->CreateBr(writeRawInvokeBlock);

		////		builder->SetInsertPoint(optimisticSignatureMatchBlock);
		////		if (NomCastStats)
		////		{
		////			builder->CreateCall(GetIncCastingMonoCastsFunction(*fun->getParent()), {});
		////		}
		////		vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadCastingVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
		////		builder->CreateBr(writeRawInvokeBlock);

		////		builder->SetInsertPoint(writeVTableBlock);
		////		specializedVtable = vtablePHI;
		////		auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
		////		auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////		builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
		////	}
		////}

		//void GenerateMonotonicStructCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* sTable, llvm::Value* rightType, llvm::Value* rtstructinterface, llvm::Value* typeArgsPtr, llvm::Value* castTypeSubsts, bool guaranteedNoRawInvoke)
		//{
		//	llvm::Module& mod = *fun->getParent();
		//	auto getCastTypeCountFun = RTConcurrentDictionaryGetCastTypeCount::Instance().GetLLVMElement(mod);
		//	auto addCastTypeFun = RTConcurrentDictionaryAddCastType::Instance().GetLLVMElement(mod);
		//	auto getCastTypeFun = RTConcurrentDictionaryGetCastType::Instance().GetLLVMElement(mod);
		//	auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
		//	auto signatureSubtyping = RTSignature::Instance().GetLLVMElement(*fun->getParent()); //TODO: check signature of raw invoke, if present
		//	auto subtypingFun = RTSubtyping::Instance().GetLLVMElement(*fun->getParent());
		//	auto dict = StructHeader::GenerateReadStructDictionary(builder, value);
		//	auto castTypeCount = builder->CreateCall(getCastTypeCountFun, { dict });
		//	castTypeCount->setCallingConv(getCastTypeCountFun->getCallingConv());

		//	BasicBlock* incomingBlock = builder->GetInsertBlock();
		//	BasicBlock* castCheckLoopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "castCheckLoop$head", fun);
		//	BasicBlock* castCheckLoopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "castCheckLoop$body", fun);
		//	BasicBlock* allCheckedBlock = BasicBlock::Create(LLVMCONTEXT, "allEntriesChecked", fun);
		//	builder->CreateBr(castCheckLoopHeadBlock);

		//	builder->SetInsertPoint(castCheckLoopHeadBlock);
		//	auto castTypeCountPHI = builder->CreatePHI(castTypeCount->getType(), 3, "#castTypes");
		//	auto castTypeIndexPHI = builder->CreatePHI(castTypeCount->getType(), 3, "castTypeIndex");
		//	castTypeCountPHI->addIncoming(castTypeCount, incomingBlock);
		//	castTypeIndexPHI->addIncoming(MakeIntLike(castTypeCount, 0), incomingBlock);
		//	auto nextCastIndex = builder->CreateAdd(castTypeIndexPHI, MakeIntLike(castTypeIndexPHI, 1));
		//	builder->CreateCondBr(builder->CreateICmpSLT(castTypeIndexPHI, castTypeCountPHI), castCheckLoopBodyBlock, allCheckedBlock);

		//	builder->SetInsertPoint(castCheckLoopBodyBlock);
		//	auto currentCastType = builder->CreateCall(getCastTypeFun, { dict, castTypeIndexPHI });
		//	currentCastType->setCallingConv(getCastTypeFun->getCallingConv());
		//	auto currentSubtype = builder->CreateCall(subtypingFun, { currentCastType, rightType, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), castTypeSubsts });
		//	currentSubtype->setCallingConv(subtypingFun->getCallingConv());
		//	castTypeCountPHI->addIncoming(castTypeCountPHI, builder->GetInsertBlock());
		//	castTypeIndexPHI->addIncoming(nextCastIndex, builder->GetInsertBlock());
		//	builder->CreateCondBr(builder->CreateICmpEQ(currentSubtype, MakeUInt(2, 3)), successBlock, castCheckLoopHeadBlock);

		//	builder->SetInsertPoint(allCheckedBlock);
		//	auto uniquedType = builder->CreateCall(typeUniqueFun, { rightType, castTypeSubsts });
		//	uniquedType->setCallingConv(typeUniqueFun->getCallingConv());
		//	auto addTypeCall = builder->CreateCall(addCastTypeFun, { dict, castTypeCountPHI, uniquedType });
		//	addTypeCall->setCallingConv(addCastTypeFun->getCallingConv());
		//	auto addTypeCallSuccess = builder->CreateICmpEQ(addTypeCall, castTypeCountPHI);
		//	castTypeCountPHI->addIncoming(addTypeCall, builder->GetInsertBlock());
		//	castTypeIndexPHI->addIncoming(castTypeIndexPHI, builder->GetInsertBlock());
		//	builder->CreateCondBr(addTypeCallSuccess, successBlock, castCheckLoopHeadBlock);
		//}
		////void GenerateMonotonicStructCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* sTable, llvm::Value* rtstructinterface, llvm::Value* typeArgsPtr, llvm::Value* castTypeSubsts, bool useCastSiteID, bool guaranteedNoTypeArgs, bool guaranteedNoRawInvoke)
		////{
		////	unsigned int extractIndex[2];
		////	extractIndex[0] = 0;
		////	extractIndex[1] = 1;
		////	auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
		////	BasicBlock* multiCastBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "multicasting structures not implemented!"); //BasicBlock::Create(LLVMCONTEXT, "multiCastCheck", fun);
		////	llvm::Value* specializedVtable = nullptr;
		////	if (!guaranteedNoTypeArgs) //still possibly no type args
		////	{

		////		BasicBlock* checkSpecializedVTableBlock = nullptr;
		////		BasicBlock* checkFreeSlotsBlock = nullptr;

		////		BasicBlock* checkNoTypeArgsBlock = builder->GetInsertBlock();
		////		BasicBlock* checkTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgs", fun);
		////		BasicBlock* checkTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgsLoop", fun);
		////		BasicBlock* copyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "copyVTable", fun);

		////		BasicBlock** firstRegularBlock = PreferTypeArgumentsInVTables ? &checkSpecializedVTableBlock : &checkFreeSlotsBlock;
		////		BasicBlock** secondRegularBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &checkSpecializedVTableBlock;

		////		BasicBlock** checkSpecializeVTableFailBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &copyVTableBlock;
		////		BasicBlock** checkFreeSlotsFailBlock = PreferTypeArgumentsInVTables ? &copyVTableBlock : &checkSpecializedVTableBlock;

		////		BasicBlock* castSiteMatch = BasicBlock::Create(LLVMCONTEXT, "castSiteMatch", fun);
		////		BasicBlock* writeOptimizedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "justWriteInVtable", fun);

		////		*firstRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkSpecializedVTable" : "checkFreeSlots", fun);
		////		*secondRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkFreeSlots" : "checkSpecializedVTable", fun);

		////		llvm::Value* rtstruct = nullptr;

		////		llvm::ConstantInt* castID = nullptr;
		////		//check cast site match (only for fully statically known types without variables)
		////		if (useCastSiteID)
		////		{
		////			checkNoTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkNoTypeArgs", fun);
		////			rtstruct = StructHeader::GenerateReadStructDescriptor(builder, value);
		////			auto specializedCastID = RTStruct::GenerateReadSpecializedVTableCastID(builder, rtstruct);
		////			castID = MakeInt(RTCast::nextCastSiteID());
		////			auto specializedTypeMatch = builder->CreateICmpEQ(specializedCastID, castID, "castSiteIsMatch");

		////			builder->CreateCondBr(specializedTypeMatch, castSiteMatch, checkNoTypeArgsBlock);
		////		}

		////		builder->SetInsertPoint(checkNoTypeArgsBlock);
		////		llvm::Value* targcount = RTInterface::GenerateReadTypeArgCount(builder, rtstructinterface);
		////		auto noTypeArgs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0), "typeArgCountIsZero");
		////		builder->CreateCondBr(noTypeArgs, writeOptimizedVTableBlock, *firstRegularBlock);

		////		// check specialized vtable
		////		{
		////			builder->SetInsertPoint(checkSpecializedVTableBlock);
		////			BasicBlock* specializedVTableExistsBlock = BasicBlock::Create(LLVMCONTEXT, "specializedVTableExists", fun);
		////			if (rtstruct == nullptr)
		////			{
		////				rtstruct = StructHeader::GenerateReadStructDescriptor(builder, value);
		////			}
		////			specializedVtable = RTStruct::GenerateReadSpecializedVTable(builder, rtstruct);
		////			builder->CreateCondBr(builder->CreateIsNotNull(specializedVtable), specializedVTableExistsBlock, *checkSpecializeVTableFailBlock);

		////			builder->SetInsertPoint(specializedVTableExistsBlock);
		////			auto origInterface = RTStructInterface::GetOriginalInterface(builder, specializedVtable);
		////			auto interfacesMatch = CreatePointerEq(builder, origInterface, rtstructinterface);
		////			auto negTypeArgCount = builder->CreateNeg(targcount);
		////			auto castTypeSubstsTypes = MakeLoad(builder, castTypeSubsts, MakeInt32(TypeArgumentListStackFields::Types));
		////			auto pointerEqCheckBlock = builder->GetInsertBlock();
		////			builder->CreateCondBr(interfacesMatch, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

		////			builder->SetInsertPoint(checkTypeArgsBlock);
		////			PHINode* argPosPHI = builder->CreatePHI(inttype(32), 2, "typeArgPos");
		////			argPosPHI->addIncoming(negTypeArgCount, pointerEqCheckBlock);
		////			auto typeArgsFinished = builder->CreateICmpEQ(argPosPHI, MakeInt32(0), "typeArgsFinished");
		////			builder->CreateCondBr(typeArgsFinished, checkTypeArgsLoopBlock, castSiteMatch);

		////			builder->SetInsertPoint(checkTypeArgsLoopBlock);
		////			auto typeEqFun = RTTypeEq::Instance().GetLLVMElement(*fun->getParent());
		////			auto leftTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, argPosPHI), "leftTypeArg");
		////			auto rightTypeArg = RTStructInterface::GetTypeArgumentNegativeIndex(builder, specializedVtable, argPosPHI);
		////			auto typeEqResult = builder->CreateCall(typeEqFun, { leftTypeArg, castTypeSubstsTypes, rightTypeArg });
		////			typeEqResult->setCallingConv(typeEqFun->getCallingConv());
		////			argPosPHI->addIncoming(builder->CreateAdd(argPosPHI, MakeInt32(1)), builder->GetInsertBlock());
		////			builder->CreateCondBr(typeEqResult, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

		////		}

		////		//check free slots (at this point we know that targcount>0)
		////		{
		////			builder->SetInsertPoint(checkFreeSlotsBlock);
		////			BasicBlock* enoughFreeSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "enoughFreeTypeArgSlots", fun);
		////			BasicBlock* copyOtherTypeArgumentsLoopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Head", fun);
		////			BasicBlock* copyOtherTypeArgumentsLoopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Body", fun);
		////			auto freeSlots = RefValueHeader::GenerateGetReserveTypeArgsFromVTablePointer(builder, vtable);
		////			auto enoughFreeSlots = builder->CreateICmpULE(targcount, freeSlots, "enoughFreeSlots");
		////			builder->CreateCondBr(enoughFreeSlots, enoughFreeSlotsBlock, *checkFreeSlotsFailBlock);

		////			builder->SetInsertPoint(enoughFreeSlotsBlock);
		////			if (NomCastStats)
		////			{
		////				builder->CreateCall(GetIncEnoughSpaceCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
		////			}
		////			auto firstIndex = MakeInt32(-1);
		////			auto firstTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, firstIndex));
		////			auto firstUniquedTarg = builder->CreateCall(typeUniqueFun, { firstTarg, castTypeSubsts });
		////			firstUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			auto cmpxchgresult = StructHeader::GenerateWriteCastTypeArgumentCMPXCHG(builder, value, MakeInt32(0), firstUniquedTarg);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, copyOtherTypeArgumentsLoopHeadBlock, multiCastBlock);

		////			builder->SetInsertPoint(copyOtherTypeArgumentsLoopHeadBlock);
		////			auto moreThanOneTypeArg = builder->CreateICmpUGT(targcount, MakeIntLike(targcount, 1));
		////			builder->CreateCondBr(moreThanOneTypeArg, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);

		////			builder->SetInsertPoint(copyOtherTypeArgumentsLoopBodyBlock);
		////			auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
		////			targposPHI->addIncoming(MakeInt32(1), copyOtherTypeArgumentsLoopHeadBlock);
		////			auto nextTargPos = builder->CreateAdd(targposPHI, MakeInt32(1));
		////			targposPHI->addIncoming(nextTargPos, copyOtherTypeArgumentsLoopBodyBlock);

		////			auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, builder->CreateSub(MakeInt32(-1), targposPHI)));
		////			auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, castTypeSubsts });
		////			currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			StructHeader::GenerateWriteCastTypeArgument(builder, value, targposPHI, currentUniquedTarg); //no cmpxchgs necessary here, as the first one serves as lock

		////			auto hasMoreTypeArgs = builder->CreateICmpUGT(targcount, nextTargPos);
		////			builder->CreateCondBr(hasMoreTypeArgs, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);
		////		}

		////		//copy vtable
		////		{
		////			BasicBlock* copyTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "copyTypeArgsToVtable", fun);
		////			BasicBlock* writeVtableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCopiedVtablePtr", fun);
		////			BasicBlock* adjustStructDataBlock = BasicBlock::Create(LLVMCONTEXT, "adjustStructData", fun);
		////			BasicBlock* tryWriteSpecializedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "tryWriteSpecializedVTable", fun);
		////			BasicBlock* checkEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "checkEnoughPreallocatedSlots", fun);
		////			BasicBlock* notEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "notEnoughPreallocatedSlots", fun);
		////			builder->SetInsertPoint(copyVTableBlock);
		////			auto newVtable = RTStructInterface::CreateCopyVTable(builder, rtstructinterface);
		////			auto typeArgsTarget = RTStructInterface::GetTypeArgumentsPointer(builder, newVtable);
		////			copyVTableBlock = builder->GetInsertBlock();
		////			builder->CreateBr(copyTypeArgsBlock);

		////			builder->SetInsertPoint(copyTypeArgsBlock);
		////			auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
		////			targposPHI->addIncoming(MakeInt32(-1), copyVTableBlock);

		////			auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, targposPHI));
		////			auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, castTypeSubsts });
		////			currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
		////			MakeStore(builder, currentUniquedTarg, builder->CreatePointerCast(newVtable, RTStructInterface::GetLLVMType()->getPointerTo()), { MakeInt32(RTStructInterfaceFields::TypeArgs), targposPHI });

		////			auto nextTargPos = builder->CreateSub(targposPHI, MakeInt32(1));
		////			targposPHI->addIncoming(nextTargPos, builder->GetInsertBlock());
		////			auto targsDone = builder->CreateICmpEQ(targposPHI, builder->CreateNeg(targcount));
		////			builder->CreateCondBr(targsDone, writeVtableBlock, copyTypeArgsBlock);

		////			builder->SetInsertPoint(writeVtableBlock);
		////			auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, newVtable, vtable);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, adjustStructDataBlock, multiCastBlock);

		////			builder->SetInsertPoint(adjustStructDataBlock);
		////			auto slotsAddress = builder->CreateGEP(builder->CreatePointerCast(rtstruct, RTStruct::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTStructFields::PreallocatedSlots) });
		////			auto typeArgSlotsCount = MakeLoad(builder, slotsAddress);
		////			auto extTargCount = builder->CreateZExtOrTrunc(targcount, typeArgSlotsCount->getType());
		////			auto specializedVtableIsNull = builder->CreateIsNull(specializedVtable, "specializedVTableIsNull");
		////			builder->CreateCondBr(specializedVtableIsNull, tryWriteSpecializedVTableBlock, checkEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(tryWriteSpecializedVTableBlock);
		////			RTStruct::GenerateWriteSpecializedVTable(builder, rtstruct, castID, newVtable);
		////			tryWriteSpecializedVTableBlock = builder->GetInsertBlock();
		////			builder->CreateBr(checkEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(checkEnoughPreallocatedSlotsBlock);
		////			auto currentPreAllocatedSlotsCount = builder->CreatePHI(typeArgSlotsCount->getType(), 3);
		////			currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, tryWriteSpecializedVTableBlock);
		////			currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, adjustStructDataBlock);
		////			auto enoughPreallocatedSlots = builder->CreateICmpULE(extTargCount, currentPreAllocatedSlotsCount, "enoughPreallocatedSlots");
		////			builder->CreateCondBr(enoughPreallocatedSlots, successBlock, notEnoughPreallocatedSlotsBlock);

		////			builder->SetInsertPoint(notEnoughPreallocatedSlotsBlock);
		////			auto updatePreallocatedSlotsResult = builder->CreateAtomicCmpXchg(slotsAddress, typeArgSlotsCount, extTargCount, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		////			currentPreAllocatedSlotsCount->addIncoming(builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex, 1)), builder->GetInsertBlock());
		////			auto updateSuccess = builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(updateSuccess, successBlock, checkEnoughPreallocatedSlotsBlock);
		////		}

		////		// cast site match
		////		{
		////			builder->SetInsertPoint(castSiteMatch);

		////			if (!guaranteedNoRawInvoke)
		////			{
		////				//todo: handle raw invoke wrappers
		////			}

		////			specializedVtable = RTStruct::GenerateReadSpecializedVTable(builder, rtstruct);
		////			auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
		////			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////			builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
		////		}

		////		builder->SetInsertPoint(writeOptimizedVTableBlock);
		////	}

		////	if (!guaranteedNoRawInvoke)
		////	{
		////		//todo: handle raw invoke wrappers
		////	}
		////	specializedVtable = RTGeneralInterface::GenerateReadOptimizedVTable(builder, rtstructinterface);
		////	auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
		////	auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
		////	builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
		////}



		llvm::FunctionType* RTCast::GetAdjustFunctionType()
		{
			static auto ft = FunctionType::get(BOOLTYPE, ArrayRef<Type*>({ REFTYPE, TYPETYPE }), false);
			return ft;
		}

		llvm::Value* GenerateEnvSubstitutions(NomBuilder& builder, CompileEnv* env, BasicBlock** outBlocks, int outBlockCount, NomTypeRef type = nullptr)
		{
			auto targcount = env->GetEnvTypeArgumentCount() + env->GetLocalTypeArgumentCount();
			if (targcount > 0 && (type == nullptr || type->ContainsVariables()))
			{
				if (env->GetLocalTypeArgumentCount() > 0)
				{
					auto targarr = builder->CreateAlloca(TYPETYPE, MakeInt(targcount), "targarr");
					RTSubstStackValue rtss = RTSubstStackValue(builder, builder->CreateGEP(targarr, MakeInt32(targcount)), nullptr, MakeInt32(targcount), targarr);
					for (int32_t i = 0; i < targcount; i++)
					{
						if (type == nullptr || type->ContainsVariableIndex(i))
						{
							MakeInvariantStore(builder, env->GetTypeArgument(builder, i), builder->CreateGEP(targarr, MakeInt32(targcount - (i + 1))));
						}
					}
					rtss.MakeTypeListInvariant(builder);
					BasicBlock* currentBlock = builder->GetInsertBlock();

					for (int i = 0; i < outBlockCount; i++)
					{
						BasicBlock* newOutBlock = BasicBlock::Create(LLVMCONTEXT, "outReleaseSubstStack", currentBlock->getParent());
						builder->SetInsertPoint(newOutBlock);
						rtss.MakeRelease(builder);
						builder->CreateBr(outBlocks[i]);

						outBlocks[i] = newOutBlock;
					}
					builder->SetInsertPoint(currentBlock);

					return rtss;
				}
				else
				{
					RTSubstStackValue rtss = RTSubstStackValue(builder, env->GetEnvTypeArgumentArray(builder));
					BasicBlock* currentBlock = builder->GetInsertBlock();

					for (int i = 0; i < outBlockCount; i++)
					{
						BasicBlock* newOutBlock = BasicBlock::Create(LLVMCONTEXT, "outReleaseSubstStack", currentBlock->getParent());
						builder->SetInsertPoint(newOutBlock);
						rtss.MakeRelease(builder);
						builder->CreateBr(outBlocks[i]);

						outBlocks[i] = newOutBlock;
					}
					builder->SetInsertPoint(currentBlock);

					return rtss;
				}
			}
			else
			{
				return ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo());
			}
		}

		llvm::Value* RTCast::GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, NomClassTypeRef type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* outTrueBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutTrue", fun);
			BasicBlock* outFalseBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutFalse", fun);

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOut", fun);

			PHINode* outPHI;
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(BOOLTYPE, 2, "castSuccess");

				builder->SetInsertPoint(outTrueBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 1), outTrueBlock);

				builder->SetInsertPoint(outFalseBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 0), outFalseBlock);
			}

			BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			builder->SetInsertPoint(origBlock);

			if (NomCastStats)
			{
				builder->CreateCall(GetIncMonotonicCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			auto stackPtr = builder->CreateIntrinsic(Intrinsic::stacksave, {}, {});

			int valueCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			SmallVector<tuple<BasicBlock*, Value*, Value*>, 8> targsInObjectSources;
			Value* vtableVar, * sTableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);

				BasicBlock* classBlock = nullptr, * structuralValueBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateRefValueKindSwitch(builder, value, &vtableVar, &sTableVar, &classBlock, &structuralValueBlock);

				//Already casted objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (classBlock != nullptr)
				{
					targsInObjectSources.push_back(make_tuple(classBlock, *value, vtableVar));
					//BasicBlock* classObjectBlock = nullptr, * lambda_targsInObject_Block = nullptr, * lambda_targsInVTable_Block = nullptr, * struct_targsInObject_Block = nullptr, * struct_targsInVTable_Block = nullptr, * partialAppBlock = nullptr, * multiCastBlock = nullptr;
					//builder->SetInsertPoint(classBlock);
					//int vtableKindCases = RTVTable::GenerateVTableKindSwitch(builder, vtableVar, &classObjectBlock, &lambda_targsInObject_Block, &lambda_targsInVTable_Block, &struct_targsInObject_Block, &struct_targsInVTable_Block, &partialAppBlock, &multiCastBlock);

					//if (classObjectBlock != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(classObjectBlock, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (lambda_targsInObject_Block != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(lambda_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (struct_targsInObject_Block != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(struct_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (lambda_targsInVTable_Block != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(lambda_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (struct_targsInVTable_Block != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(struct_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (partialAppBlock != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(partialAppBlock, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (multiCastBlock != nullptr)
					//{
					//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting of multicasted objects not implemented!", multiCastBlock); //TODO: implement
					//}
				}

				if (structuralValueBlock != nullptr)
				{
					builder->SetInsertPoint(structuralValueBlock);
					BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
					auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
					StructuralValueHeader::GenerateMonotonicStructuralCast(builder, env, fun, outBlocks[0], outBlocks[1], value, type, substStack);
				}

				////PURE LAMBDA
				//if (lambdaBlock != nullptr)
				//{
				//	builder->SetInsertPoint(lambdaBlock);
				//	if (!type->Named->IsInterface())
				//	{
				//		builder->CreateBr(outFalseBlock);
				//	}
				//	else if (!type->Named->IsFunctional())
				//	{
				//		builder->CreateBr(outFalseBlock);
				//	}
				//	else
				//	{
				//		GenerateMonotonicLambdaCast(builder, env, fun, outTrueBlock, outFalseBlock, value, sTableVar, type->GetLLVMElement(*fun->getParent()), type->Named->GetLLVMElement(*fun->getParent()), RTClassType::GetTypeArgumentsPtr(builder, type->GetLLVMElement(*fun->getParent())), GenerateEnvSubstitutions(builder, env, type), type->Named->GetTypeParametersCount() == 0);
				//	}
				//}

				////PURE STRUCT
				//if (structBlock != nullptr)
				//{
				//	builder->SetInsertPoint(structBlock);
				//	if (!type->Named->IsInterface())
				//	{
				//		builder->CreateBr(outFalseBlock);
				//	}
				//	else
				//	{
				//		auto envSubsts = GenerateEnvSubstitutions(builder, env, type);
				//		GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, type->GetLLVMElement(*fun->getParent()), type->Named->GetLLVMElement(*fun->getParent()), RTClassType::GetTypeArgumentsPtr(builder, type->GetLLVMElement(*fun->getParent())), envSubsts, !type->Named->GetHasRawInvoke());
				//	}
				//}

				////PARTIAL APPLICATION
				//if (partialAppBlock != nullptr)
				//{
				//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting partial applications not implemented", partialAppBlock); //TODO: implement
				//}
			}

			if (intBlock != nullptr)
			{
				builder->SetInsertPoint(intBlock);
				if (NomIntClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomIntClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (floatBlock != nullptr)
			{
				builder->SetInsertPoint(floatBlock);
				if (NomFloatClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomFloatClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				if (NomIntClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomIntClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				if (NomFloatClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomFloatClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				if (NomBoolClass::GetInstance()->GetType()->IsSubtype(type, false))
				{
					builder->CreateBr(outTrueBlock);
				}
				else if (NomBoolClass::GetInstance()->GetType()->IsDisjoint(type))
				{
					builder->CreateBr(outFalseBlock);
				}
				else
				{
					targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
				}
			}

			int nominalSubtypingCases = targsInObjectSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* ifacePtr = nullptr;
				PHINode* ifacePHI = nullptr;
				Value* typeArgsPtr = nullptr;
				PHINode* typeArgsPHI = nullptr;
				if (nominalSubtypingCases > 1)
				{
					builder->SetInsertPoint(nominalSubtypingCheck);
					ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), nominalSubtypingCases, "interfacePointer");
					typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), nominalSubtypingCases, "typeArgs");
					ifacePtr = ifacePHI;
					typeArgsPtr = typeArgsPHI;
				}
				for (auto& tpl : targsInObjectSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(std::get<1>(tpl), TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(std::get<2>(tpl), RTInterface::GetLLVMType()->getPointerTo());

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}
				//for (auto& tpl : targsInVTableSources)
				//{
				//	builder->SetInsertPoint(std::get<0>(tpl));
				//	auto typeArgsCast = builder->CreatePointerCast(vtableVar, TYPETYPE->getPointerTo());
				//	auto ifaceCast = builder->CreatePointerCast(vtableVar, RTInterface::GetLLVMType()->getPointerTo());
				//	auto adjustFun = std::get<1>(tpl);

				//	if (nominalSubtypingCases > 1)
				//	{
				//		typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
				//		ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
				//		adjustPHI->addIncoming(adjustFun, builder->GetInsertBlock());
				//	}
				//	else
				//	{
				//		typeArgsPtr = typeArgsCast;
				//		ifacePtr = ifaceCast;
				//		adjustFunction = adjustFun;
				//	}
				//	builder->CreateBr(nominalSubtypingCheck);
				//}


				builder->SetInsertPoint(nominalSubtypingCheck);
				auto instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				//auto envSubstitutions = GenerateEnvSubstitutions(builder, env, type);

				BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
				auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2, type);
				//BasicBlock* stErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Error during subtyping check!");
				RTSubtyping::CreateInlineSubtypingCheck(builder, builder->CreatePointerCast(instanceType, TYPETYPE), nullptr, type, substStack, outBlocks[0], nullptr, outBlocks[1]);
			}

			builder->SetInsertPoint(outBlock);
			builder->CreateIntrinsic(Intrinsic::stackrestore, {}, { stackPtr });
			return outPHI;
		}

		llvm::Value* RTCast::GenerateMonotonicCast(NomBuilder& builder, CompileEnv* env, NomValue& value, llvm::Value* type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* outTrueBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutTrue", fun);
			BasicBlock* outFalseBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOutFalse", fun);

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "MonotonicCastOut", fun);
			PHINode* outPHI;
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(BOOLTYPE, 2, "castSuccess");

				builder->SetInsertPoint(outTrueBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 1), outTrueBlock);

				builder->SetInsertPoint(outFalseBlock);
				builder->CreateBr(outBlock);
				outPHI->addIncoming(MakeUInt(1, 0), outFalseBlock);
			}

			BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			builder->SetInsertPoint(origBlock);
			if (NomCastStats)
			{
				builder->CreateCall(GetIncMonotonicCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}
			auto stackPtr = builder->CreateIntrinsic(Intrinsic::stacksave, {}, {});

			int valueCases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			SmallVector<tuple<BasicBlock*, Value*, Value*>, 8> targsInObjectSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);

				BasicBlock* classBlock = nullptr, * structuralValueBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateRefValueKindSwitch(builder, value, &vtableVar, nullptr, &classBlock, &structuralValueBlock);

				//Already casted objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (classBlock != nullptr)
				{
					targsInObjectSources.push_back(make_tuple(classBlock, *value, vtableVar));
					//BasicBlock* classObjectBlock = nullptr, * lambda_targsInObject_Block = nullptr, * lambda_targsInVTable_Block = nullptr, * struct_targsInObject_Block = nullptr, * struct_targsInVTable_Block = nullptr, * partialAppBlock = nullptr, * multiCastBlock = nullptr;
					//builder->SetInsertPoint(classBlock);
					//int vtableKindCases = RTVTable::GenerateVTableKindSwitch(builder, vtableVar, &classObjectBlock, &lambda_targsInObject_Block, &lambda_targsInVTable_Block, &struct_targsInObject_Block, &struct_targsInVTable_Block, &partialAppBlock, &multiCastBlock);

					//if (classObjectBlock != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(classObjectBlock, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (lambda_targsInObject_Block != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(lambda_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (struct_targsInObject_Block != nullptr)
					//{
					//	targsInObjectSources.push_back(make_tuple(struct_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (lambda_targsInVTable_Block != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(lambda_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (struct_targsInVTable_Block != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(struct_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (partialAppBlock != nullptr)
					//{
					//	targsInVTableSources.push_back(make_pair(partialAppBlock, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					//}
					//if (multiCastBlock != nullptr)
					//{
					//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting of multicasted objects not implemented!", multiCastBlock); //TODO: implement
					//}
				}

				//STRUCTURAL VALUE - note difference to inlined version above: there we know we have a class type, here we can get any type
				if (structuralValueBlock != nullptr)
				{
					builder->SetInsertPoint(structuralValueBlock);
					BasicBlock* outBlocks[2] = { outTrueBlock, outFalseBlock };
					auto substStack = GenerateEnvSubstitutions(builder, env, outBlocks, 2);
					BasicBlock* rightClassTypeBlock = nullptr, * rightInstanceTypeBlock = nullptr;
					Value* rightPHI = nullptr;
					Value* rightSubstPHI = nullptr;
					RTTypeHead::GenerateTypeKindSwitchRecurse(builder, type, substStack, &rightPHI, &rightSubstPHI, &rightClassTypeBlock, &(outBlocks[0]), &(outBlocks[1]), &(outBlocks[1]), &rightInstanceTypeBlock, &(outBlocks[0]), nullptr, outBlocks[1]);

					if (rightClassTypeBlock)
					{
						//BasicBlock* lambdaCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureLambda$classType", fun);
						builder->SetInsertPoint(rightClassTypeBlock);
						auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI);
						//auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						//auto isFunctionalInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
						//builder->CreateCondBr(isFunctionalInterface, lambdaCastBlock, outFalseBlock);

						//builder->SetInsertPoint(lambdaCastBlock);
						StructuralValueHeader::GenerateMonotonicStructuralCast(builder, env, fun, outBlocks[0], outBlocks[1], value, rightPHI, iface, RTClassType::GetTypeArgumentsPtr(builder, rightPHI), rightSubstPHI);
					}

					if (rightInstanceTypeBlock)
					{
						//BasicBlock* lambdaCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureLambda$instanceType", fun);
						builder->SetInsertPoint(rightInstanceTypeBlock);
						auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI);
						//auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						//auto isFunctionalInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
						//builder->CreateCondBr(isFunctionalInterface, lambdaCastBlock, outFalseBlock);

						//builder->SetInsertPoint(lambdaCastBlock);
						StructuralValueHeader::GenerateMonotonicStructuralCast(builder, env, fun, outBlocks[0], outBlocks[1], value, rightPHI, iface, RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), rightSubstPHI);
					}
				}

				////PURE STRUCT - note difference to inlined version above: there we know we have a class type, here we can get any type
				//if (structBlock != nullptr)
				//{
				//	BasicBlock* rightClassTypeBlock = nullptr, * rightTypeVarBlock = nullptr, * rightInstanceTypeBlock = nullptr, * rightMaybeTypeBlock = nullptr;
				//	builder->SetInsertPoint(structBlock);
				//	PHINode* rightPHI = builder->CreatePHI(type->getType(), 3, "rightType");
				//	rightPHI->addIncoming(type, structBlock->getSinglePredecessor());
				//	RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, &rightClassTypeBlock, &outTrueBlock, &rightTypeVarBlock, &outFalseBlock, &rightInstanceTypeBlock, &outFalseBlock, &outFalseBlock, &outTrueBlock, &rightMaybeTypeBlock);

				//	if (rightTypeVarBlock != nullptr)
				//	{
				//		builder->SetInsertPoint(rightTypeVarBlock);
				//		auto envSubsts = GenerateEnvSubstitutions(builder, env);
				//		auto typeList = MakeLoad(builder, envSubsts, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
				//		auto newRightType = MakeLoad(builder, builder->CreateGEP(typeList, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
				//		rightPHI->addIncoming(newRightType, builder->GetInsertBlock()); //this type should not contain any more type variables, so the code here getting the variables from the environment again should be safe
				//		builder->CreateBr(structBlock);
				//	}

				//	if (rightMaybeTypeBlock != nullptr)
				//	{
				//		builder->SetInsertPoint(rightMaybeTypeBlock);
				//		auto newRightType = RTMaybeType::GenerateReadPotentialType(builder, rightPHI);
				//		rightPHI->addIncoming(newRightType, builder->GetInsertBlock());
				//		builder->CreateBr(structBlock);
				//	}

				//	if (rightClassTypeBlock)
				//	{
				//		BasicBlock* structCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureStruct$classType", fun);
				//		builder->SetInsertPoint(rightClassTypeBlock);
				//		auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI);
				//		auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
				//		auto isInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface));
				//		builder->CreateCondBr(isInterface, structCastBlock, outFalseBlock);

				//		builder->SetInsertPoint(structCastBlock);
				//		auto envSubsts = GenerateEnvSubstitutions(builder, env);
				//		GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, sTableVar, rightPHI, iface, RTClassType::GetTypeArgumentsPtr(builder, rightPHI), envSubsts, false);
				//	}

				//	if (rightInstanceTypeBlock)
				//	{
				//		BasicBlock* structCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureStruct$instanceType", fun);
				//		builder->SetInsertPoint(rightInstanceTypeBlock);
				//		auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI);
				//		auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
				//		auto isInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface));
				//		builder->CreateCondBr(isInterface, structCastBlock, outFalseBlock);

				//		builder->SetInsertPoint(structCastBlock);
				//		auto envSubsts = GenerateEnvSubstitutions(builder, env);
				//		GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, sTableVar, rightPHI, iface, RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), envSubsts, false);
				//	}
				//}

				////PARTIAL APPLICATION
				//if (partialAppBlock != nullptr)
				//{
				//	RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting partial applications not allowed!", partialAppBlock);
				//}
			}

			if (intBlock != nullptr)
			{
				builder->SetInsertPoint(intBlock);
				targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (floatBlock != nullptr)
			{
				builder->SetInsertPoint(floatBlock);
				targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo())));
			}

			int nominalSubtypingCases = targsInObjectSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* adjustFunction = nullptr;
				Value* ifacePtr = nullptr;
				PHINode* ifacePHI = nullptr;
				Value* typeArgsPtr = nullptr;
				PHINode* typeArgsPHI = nullptr;
				if (nominalSubtypingCases > 1)
				{
					builder->SetInsertPoint(nominalSubtypingCheck);
					ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), nominalSubtypingCases, "interfacePointer");
					typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), nominalSubtypingCases, "typeArgs");
					ifacePtr = ifacePHI;
					typeArgsPtr = typeArgsPHI;
				}
				for (auto& tpl : targsInObjectSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(std::get<1>(tpl), TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(std::get<2>(tpl), RTInterface::GetLLVMType()->getPointerTo());

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}

				builder->SetInsertPoint(nominalSubtypingCheck);
				Value* instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				BasicBlock* outBlocks[2] = { outTrueBlock,outFalseBlock };
				auto envSubstitutions = GenerateEnvSubstitutions(builder, env, outBlocks, 2);
				instanceType = builder->CreateGEP(instanceType, { MakeInt32(0), MakeInt32(RTInstanceTypeFields::Head) });
				auto subtypingResult = builder->CreateCall(RTSubtyping::Instance().GetLLVMElement(*fun->getParent()), { instanceType, type, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), envSubstitutions });
				subtypingResult->setCallingConv(NOMCC);
				auto subtypingFailed = builder->CreateICmpEQ(subtypingResult, MakeUInt(2, 0));
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { subtypingFailed, MakeUInt(1,0) });
				builder->CreateCondBr(subtypingFailed, outBlocks[1], outBlocks[0]);
			}

			builder->SetInsertPoint(outBlock);
			builder->CreateIntrinsic(Intrinsic::stackrestore, {}, { stackPtr });

			return outPHI;
		}

		uint64_t RTCast::nextCastSiteID()
		{
			static uint64_t csid = 1; return csid++;
		}


		llvm::Value* RTCast::GenerateCast(NomBuilder& builder, CompileEnv* env, NomValue value, NomTypeRef type)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			if (value.GetNomType()->IsSubtype(type, false))
			{
				return MakeInt(1, (uint64_t)1);
			}
			if (NomCastStats)
			{
				builder->CreateCall(GetIncCastsFunction(*fun->getParent()), {});
			}
			switch (value.GetNomType()->GetKind())
			{
			case TypeKind::TKBottom:
				return MakeInt(1, (uint64_t)1); //this means we're in dead code, as actual values can't have this type
			case TypeKind::TKMaybe:
				if (NomNullClass::GetInstance()->GetType()->IsSubtype(type))
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto nullobjptr = builder->CreatePtrToInt(NomNullObject::GetInstance()->GetLLVMElement(*env->Module), INTTYPE);
					auto tpeq = builder->CreateICmpEQ(nullobjptr, builder->CreatePtrToInt(value, INTTYPE));
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return tpeq;
				}
				//else, fall through (note, if class types don't fall through anymore, their case needs to be moved)
			case TypeKind::TKClass:
				//todo: optimize by drilling down to how far the statically known type and target type match, for now, fall through and do everything dynamically
			default:
				switch (type->GetKind())
				{
				case TypeKind::TKBottom:
					return MakeInt(1, (uint64_t)0);
				case TypeKind::TKTop:
				case TypeKind::TKDynamic:
					throw new std::exception(); //the subtyping check at the beginning should have caught that; something's wrong if this execption is thrown
				case TypeKind::TKMaybe:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "outBlock", fun);
					BasicBlock* IsNotNullBlock = BasicBlock::Create(LLVMCONTEXT, "isNotNull", fun);
					auto nullobjptr = builder->CreatePtrToInt(NomNullObject::GetInstance()->GetLLVMElement(*env->Module), INTTYPE);
					auto nullcmp = builder->CreateICmpEQ(nullobjptr, builder->CreatePtrToInt(value, INTTYPE));
					builder->CreateCondBr(nullcmp, outBlock, IsNotNullBlock);

					builder->SetInsertPoint(IsNotNullBlock);
					auto castresult = GenerateCast(builder, env, value, ((NomMaybeTypeRef)type)->PotentialType);
					BasicBlock* notNullBlock = builder->GetInsertBlock();
					builder->CreateBr(outBlock);

					builder->SetInsertPoint(outBlock);
					auto resultPHI = builder->CreatePHI(BOOLTYPE, 2, "subtypingResult");
					resultPHI->addIncoming(MakeInt(1, (uint64_t)1), origBlock);
					resultPHI->addIncoming(castresult, notNullBlock);
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { resultPHI,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return resultPHI;
				}
				case TypeKind::TKClass:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto itype = (NomClassTypeRef)type;
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, itype);
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return monoCastResult;
				}
				case TypeKind::TKVariable:
				{
					Value* castTimeStamp = nullptr;
					if (NomCastStats)
					{
						castTimeStamp = builder->CreateCall(GetGetTimestampFunction(*fun->getParent()), {});
					}
					auto monoCastResult = GenerateMonotonicCast(builder, env, value, env->GetTypeArgument(builder, ((NomTypeVarRef)type)->GetIndex()));
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { monoCastResult,MakeUInt(1,1) });
					if (NomCastStats)
					{
						builder->CreateCall(GetIncCastTimeFunction(*fun->getParent()), { castTimeStamp });
					}
					return monoCastResult;
				}
				default:
					throw new std::exception();
				}
			}
		}

		llvm::Value* RTCast::GenerateCast(NomBuilder& builder, CompileEnv* env, llvm::Value* value, NomTypeRef type)
		{
			return GenerateCast(builder, env, NomValue(value, &NomDynamicType::Instance(), false), type);
		}
		llvm::Function* FailingAdjustFun::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = mod.getFunction("RT_NOM_CASTADJUST_FAILING");
			if (fun != nullptr)
			{
				return fun;
			}
			fun = Function::Create(RTCast::GetAdjustFunctionType(), linkage, "RT_NOM_CASTADJUST_FAILING", mod);
			fun->setCallingConv(NOMCC);
			NomBuilder builder;

			BasicBlock* start = BasicBlock::Create(LLVMCONTEXT, "", fun);
			builder->SetInsertPoint(start);
			builder->CreateRet(MakeUInt(1, 0));
			return fun;
		}
		llvm::Function* FailingAdjustFun::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_CASTADJUST_FAILING");
		}
	}
}
