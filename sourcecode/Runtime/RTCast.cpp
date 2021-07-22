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

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
	
		void GenerateMonotonicLambdaCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* vtable, llvm::Value* rtfunctionalInterface, llvm::Value* outerStack, llvm::Value* clsTypeArgCount, llvm::Value* typeArgsPtr, bool useCastSiteID, bool guaranteedNoTypeArgs)
		{
			unsigned int extractIndex[2];
			extractIndex[0] = 0;
			extractIndex[1] = 1;
			auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
			BasicBlock* multiCastBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "multicasting structures not implemented!"); //BasicBlock::Create(LLVMCONTEXT, "multiCastCheck", fun);
			llvm::Value* specializedVtable = nullptr;
			llvm::Value* rtlambda = LambdaHeader::GenerateReadLambdaMetadata(builder, value);
			auto signatureSubtyping = RTSignature::Instance().GetLLVMElement(*fun->getParent());
			if (!guaranteedNoTypeArgs)
			{
				BasicBlock* checkSpecializedVTableBlock = nullptr;
				BasicBlock* checkFreeSlotsBlock = nullptr;

				BasicBlock* checkNoTypeArgsBlock = builder->GetInsertBlock();
				BasicBlock* copyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "copyVTable", fun);

				BasicBlock** firstRegularBlock = PreferTypeArgumentsInVTables ? &checkSpecializedVTableBlock : &checkFreeSlotsBlock;
				BasicBlock** secondRegularBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &checkSpecializedVTableBlock;

				BasicBlock** checkSpecializeVTableFailBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &copyVTableBlock;
				BasicBlock** checkFreeSlotsFailBlock = PreferTypeArgumentsInVTables ? &copyVTableBlock : &checkSpecializedVTableBlock;

				*firstRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkSpecializedVTable" : "checkFreeSlots", fun);
				*secondRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkFreeSlots" : "checkSpecializedVTable", fun);

				BasicBlock* checkTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgs", fun);
				BasicBlock* checkTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgsLoop", fun);

				BasicBlock* castSiteMatch = BasicBlock::Create(LLVMCONTEXT, "castSiteMatch", fun);
				BasicBlock* writeOptimizedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeOptimizedVTable", fun);

				specializedVtable = RTLambda::GenerateReadSpecializedVTable(builder, rtlambda);
				llvm::ConstantInt* castID = nullptr;
				if (useCastSiteID)
				{
					checkNoTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkNoTypeArgs", fun);
					auto specializedCastID = RTLambda::GenerateReadSpecializedVTableCastID(builder, rtlambda);
					castID = MakeInt(RTCast::nextCastSiteID());
					auto specializedTypeMatch = builder->CreateICmpEQ(specializedCastID, castID, "castSiteIsMatch");

					builder->CreateCondBr(specializedTypeMatch, castSiteMatch, checkNoTypeArgsBlock);
				}

				builder->SetInsertPoint(checkNoTypeArgsBlock);
				auto noTypeArgs = builder->CreateICmpEQ(clsTypeArgCount, MakeIntLike(clsTypeArgCount, 0), "typeArgCountIsZero");
				builder->CreateCondBr(noTypeArgs, writeOptimizedVTableBlock, *firstRegularBlock);

				//check specialized vtable
				{
					builder->SetInsertPoint(checkSpecializedVTableBlock);
					BasicBlock* specializedVTableExistsBlock = BasicBlock::Create(LLVMCONTEXT, "specializedVTableExists", fun);

					builder->CreateCondBr(builder->CreateIsNotNull(specializedVtable), specializedVTableExistsBlock, *checkSpecializeVTableFailBlock);

					builder->SetInsertPoint(specializedVTableExistsBlock);
					auto origInterface = RTStructInterface::GetOriginalInterface(builder, specializedVtable);
					auto interfacesMatch = CreatePointerEq(builder, origInterface, rtfunctionalInterface);
					auto negTypeArgCount = builder->CreateNeg(clsTypeArgCount);
					auto castTypeSubstsTypes = MakeLoad(builder, outerStack, MakeInt32(TypeArgumentListStackFields::Types));
					auto pointerEqCheckBlock = builder->GetInsertBlock();
					builder->CreateCondBr(interfacesMatch, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

					builder->SetInsertPoint(checkTypeArgsBlock);
					PHINode* argPosPHI = builder->CreatePHI(inttype(32), 2, "typeArgPos");
					argPosPHI->addIncoming(negTypeArgCount, pointerEqCheckBlock);
					auto typeArgsFinished = builder->CreateICmpEQ(argPosPHI, MakeInt32(0), "typeArgsFinished");
					builder->CreateCondBr(typeArgsFinished, checkTypeArgsLoopBlock, castSiteMatch);

					builder->SetInsertPoint(checkTypeArgsLoopBlock);
					auto typeEqFun = RTTypeEq::Instance().GetLLVMElement(*fun->getParent());
					auto leftTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, argPosPHI), "leftTypeArg");
					auto rightTypeArg = RTLambdaInterface::GetTypeArgumentNegativeIndex(builder, specializedVtable, argPosPHI);
					auto typeEqResult = builder->CreateCall(typeEqFun, { leftTypeArg, castTypeSubstsTypes, rightTypeArg });
					typeEqResult->setCallingConv(typeEqFun->getCallingConv());
					argPosPHI->addIncoming(builder->CreateAdd(argPosPHI, MakeInt32(1)), builder->GetInsertBlock());
					builder->CreateCondBr(typeEqResult, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);
				}

				//check free slots (at this point we know that targcount>0)
				{
					builder->SetInsertPoint(checkFreeSlotsBlock);
					BasicBlock* enoughFreeSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "enoughFreeTypeArgSlots", fun);
					BasicBlock* copyOtherTypeArgumentsLoopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Head", fun);
					BasicBlock* copyOtherTypeArgumentsLoopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Body", fun);
					auto freeSlots = RefValueHeader::GenerateGetReserveTypeArgsFromVTablePointer(builder, vtable);
					auto enoughFreeSlots = builder->CreateICmpULE(clsTypeArgCount, freeSlots, "enoughFreeSlots");
					builder->CreateCondBr(enoughFreeSlots, enoughFreeSlotsBlock, *checkFreeSlotsFailBlock);

					builder->SetInsertPoint(enoughFreeSlotsBlock);
					if (NomCastStats)
					{
						builder->CreateCall(GetIncEnoughSpaceCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
					auto firstIndex = MakeInt32(-1);
					auto firstTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, firstIndex));
					auto firstUniquedTarg = builder->CreateCall(typeUniqueFun, { firstTarg, outerStack });
					firstUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					auto cmpxchgresult = LambdaHeader::GenerateWriteCastTypeArgumentCMPXCHG(builder, value, MakeInt32(0), firstUniquedTarg);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, copyOtherTypeArgumentsLoopHeadBlock, multiCastBlock);

					builder->SetInsertPoint(copyOtherTypeArgumentsLoopHeadBlock);
					auto moreThanOneTypeArg = builder->CreateICmpUGT(clsTypeArgCount, MakeIntLike(clsTypeArgCount, 1));
					builder->CreateCondBr(moreThanOneTypeArg, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);

					builder->SetInsertPoint(copyOtherTypeArgumentsLoopBodyBlock);
					auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
					targposPHI->addIncoming(MakeInt32(1), copyOtherTypeArgumentsLoopHeadBlock);
					auto nextTargPos = builder->CreateAdd(targposPHI, MakeInt32(1));
					targposPHI->addIncoming(nextTargPos, copyOtherTypeArgumentsLoopBodyBlock);

					auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, builder->CreateSub(MakeInt32(-1), targposPHI)));
					auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, outerStack });
					currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					LambdaHeader::GenerateWriteCastTypeArgument(builder, value, targposPHI, currentUniquedTarg); //no cmpxchgs necessary here, as the first one serves as lock

					auto hasMoreTypeArgs = builder->CreateICmpUGT(clsTypeArgCount, nextTargPos);
					builder->CreateCondBr(hasMoreTypeArgs, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);
				}


				//copy vtable
				{
					BasicBlock* doCopyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "doCopyVTableBlock", fun);
					builder->SetInsertPoint(doCopyVTableBlock);
					PHINode* vtablePHI = builder->CreatePHI(RTLambdaInterface::GetLLVMType()->getPointerTo(), 3);
					//block to be continued further below

					builder->SetInsertPoint(copyVTableBlock);
					{
						BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
						BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
						BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
						BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);

						auto lambdaSig = RTLambda::GenerateReadSignature(builder, rtlambda);
						auto interfaceSig = RTFunctionalInterface::GenerateReadRTSignature(builder, rtfunctionalInterface);

						auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
						MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
						MakeStore(builder, typeArgsPtr, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

						auto signatureMatch = builder->CreateCall(signatureSubtyping, { lambdaSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
						signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
						auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
						signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
						signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

						builder->SetInsertPoint(pessimisticSignatureMatchBlock);
						if (NomCastStats)
						{
							builder->CreateCall(GetIncCheckedMonoCastsFunction(*fun->getParent()), {});
						}
						auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, lambdaSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
						builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

						builder->SetInsertPoint(prototypeMatchBlock);
						vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMatchVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
						builder->CreateBr(doCopyVTableBlock);

						builder->SetInsertPoint(prototypeMismatchBlock);
						vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMismatchVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
						builder->CreateBr(doCopyVTableBlock);

						builder->SetInsertPoint(optimisticSignatureMatchBlock);
						if (NomCastStats)
						{
							builder->CreateCall(GetIncCastingMonoCastsFunction(*fun->getParent()), {});
						}
						vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadCastingVTable(builder, rtfunctionalInterface), builder->GetInsertBlock());
						builder->CreateBr(doCopyVTableBlock);

					}

					BasicBlock* copyTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "copyTypeArgsToVtable", fun);
					BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCopiedVTablePtr", fun);
					BasicBlock* writeRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "writeRawInvoke", fun);
					BasicBlock* adjustStructDataBlock = BasicBlock::Create(LLVMCONTEXT, "adjustStructData", fun);
					BasicBlock* tryWriteSpecializedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "tryWriteSpecializedVTable", fun);
					BasicBlock* checkEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "checkEnoughPreallocatedSlots", fun);
					BasicBlock* notEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "notEnoughPreallocatedSlots", fun);

					builder->SetInsertPoint(doCopyVTableBlock);
					auto newVtable = RTLambdaInterface::CreateCopyVTable(builder, vtablePHI, clsTypeArgCount);
					auto typeArgsTarget = RTLambdaInterface::GetTypeArgumentsPointer(builder, newVtable);
					copyVTableBlock = builder->GetInsertBlock();
					builder->CreateBr(copyTypeArgsBlock);

					builder->SetInsertPoint(copyTypeArgsBlock);
					auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
					targposPHI->addIncoming(MakeInt32(-1), copyVTableBlock);

					auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, targposPHI));
					auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, outerStack });
					currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					MakeStore(builder, currentUniquedTarg, builder->CreatePointerCast(newVtable, RTLambdaInterface::GetLLVMType()->getPointerTo()), { MakeInt32(RTLambdaInterfaceFields::TypeArgs), targposPHI });

					auto nextTargPos = builder->CreateSub(targposPHI, MakeInt32(1));
					targposPHI->addIncoming(nextTargPos, builder->GetInsertBlock());
					auto targsDone = builder->CreateICmpEQ(targposPHI, builder->CreateNeg(clsTypeArgCount));
					builder->CreateCondBr(targsDone, writeRawInvokeBlock, copyTypeArgsBlock);

					builder->SetInsertPoint(writeRawInvokeBlock);
					auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, newVtable);
					auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
					auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

					builder->SetInsertPoint(writeVTableBlock);
					auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, newVtable, vtable);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, adjustStructDataBlock, multiCastBlock);

					builder->SetInsertPoint(adjustStructDataBlock);
					auto slotsAddress = builder->CreateGEP(builder->CreatePointerCast(rtlambda, RTLambda::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTLambdaFields::PreallocatedSlots) });
					auto typeArgSlotsCount = MakeLoad(builder, slotsAddress);
					auto extTargCount = builder->CreateZExtOrTrunc(clsTypeArgCount, typeArgSlotsCount->getType());
					auto specializedVtableIsNull = builder->CreateIsNull(specializedVtable, "specializedVTableIsNull");
					builder->CreateCondBr(specializedVtableIsNull, tryWriteSpecializedVTableBlock, checkEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(tryWriteSpecializedVTableBlock);
					RTLambda::GenerateWriteSpecializedVTable(builder, rtlambda, castID, newVtable);
					tryWriteSpecializedVTableBlock = builder->GetInsertBlock();
					builder->CreateBr(checkEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(checkEnoughPreallocatedSlotsBlock);
					auto currentPreAllocatedSlotsCount = builder->CreatePHI(typeArgSlotsCount->getType(), 3);
					currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, tryWriteSpecializedVTableBlock);
					currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, adjustStructDataBlock);
					auto enoughPreallocatedSlots = builder->CreateICmpULE(extTargCount, currentPreAllocatedSlotsCount, "enoughPreallocatedSlots");
					builder->CreateCondBr(enoughPreallocatedSlots, successBlock, notEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(notEnoughPreallocatedSlotsBlock);
					auto updatePreallocatedSlotsResult = builder->CreateAtomicCmpXchg(slotsAddress, typeArgSlotsCount, extTargCount, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
					currentPreAllocatedSlotsCount->addIncoming(builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex, 1)), builder->GetInsertBlock());
					auto updateSuccess = builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(updateSuccess, successBlock, checkEnoughPreallocatedSlotsBlock);
				}

				// cast site match
				{
					BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCastSiteMatchVtable", fun);
					builder->SetInsertPoint(castSiteMatch);
					specializedVtable = RTLambda::GenerateReadSpecializedVTable(builder, rtlambda);

					auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, specializedVtable);
					auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
					auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

					builder->SetInsertPoint(writeVTableBlock);
					auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
				}

				builder->SetInsertPoint(writeOptimizedVTableBlock);
			}

			{
				BasicBlock* pessimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "pessimisticSignatureMatch", fun);
				BasicBlock* optimisticSignatureMatchBlock = BasicBlock::Create(LLVMCONTEXT, "optimisticSignatureMatch", fun);
				BasicBlock* prototypeMatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMatch", fun);
				BasicBlock* prototypeMismatchBlock = BasicBlock::Create(LLVMCONTEXT, "prototypeMismatch", fun);
				BasicBlock* writeRawInvokeBlock = BasicBlock::Create(LLVMCONTEXT, "writeRawInvoke", fun);
				BasicBlock* writeVTableBlock = BasicBlock::Create(LLVMCONTEXT, "writeVTable", fun);

				auto lambdaSig = RTLambda::GenerateReadSignature(builder, rtlambda);
				auto interfaceSig = RTFunctionalInterface::GenerateReadRTSignature(builder, rtfunctionalInterface);

				auto sigSubst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType());
				MakeStore(builder, outerStack, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Next) }));
				MakeStore(builder, typeArgsPtr, builder->CreateGEP(sigSubst, { MakeInt32(0), MakeInt32(TypeArgumentListStackFields::Types) }));

				auto signatureMatch = builder->CreateCall(signatureSubtyping, { lambdaSig, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), interfaceSig, sigSubst }, "signatureMatch");
				signatureMatch->setCallingConv(signatureSubtyping->getCallingConv());
				auto signatureMatchSwitch = builder->CreateSwitch(signatureMatch, failBlock, 2);
				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)3), pessimisticSignatureMatchBlock);
				signatureMatchSwitch->addCase(MakeInt(2, (uint64_t)1), optimisticSignatureMatchBlock);

				builder->SetInsertPoint(writeRawInvokeBlock);
				auto vtablePHI = builder->CreatePHI(RTLambdaInterface::GetLLVMType()->getPointerTo(), 3);
				auto rawInvokeFun = RTLambdaInterface::GetRawInvokeWrapper(builder, vtablePHI);
				auto ricmpxchgresult = RefValueHeader::GenerateWriteRawInvokeCMPXCHG(builder, value, ConstantPointerNull::get(POINTERTYPE), rawInvokeFun);
				auto ricmpxchgsuccess = builder->CreateExtractValue(ricmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
				builder->CreateCondBr(ricmpxchgsuccess, writeVTableBlock, multiCastBlock);

				builder->SetInsertPoint(pessimisticSignatureMatchBlock);
				if (NomCastStats)
				{
					builder->CreateCall(GetIncCheckedMonoCastsFunction(*fun->getParent()), {});
				}
				auto protoMatch = builder->CreateICmpEQ(builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, lambdaSig), numtype(intptr_t)), builder->CreatePtrToInt(RTSignature::GenerateReadLLVMFunctionType(builder, interfaceSig), numtype(intptr_t)), "protoMatch");
				builder->CreateCondBr(protoMatch, prototypeMatchBlock, prototypeMismatchBlock);

				builder->SetInsertPoint(prototypeMatchBlock);
				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMatchVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
				builder->CreateBr(writeRawInvokeBlock);

				builder->SetInsertPoint(prototypeMismatchBlock);
				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadProtoMismatchVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
				builder->CreateBr(writeRawInvokeBlock);

				builder->SetInsertPoint(optimisticSignatureMatchBlock);
				if (NomCastStats)
				{
					builder->CreateCall(GetIncCastingMonoCastsFunction(*fun->getParent()), {});
				}
				vtablePHI->addIncoming(RTFunctionalInterface::GenerateReadCastingVTableOpt(builder, rtfunctionalInterface), builder->GetInsertBlock());
				builder->CreateBr(writeRawInvokeBlock);

				builder->SetInsertPoint(writeVTableBlock);
				specializedVtable = vtablePHI;
				auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
				auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
				builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
			}
		}

		void GenerateMonotonicStructCast(NomBuilder& builder, CompileEnv* env, llvm::Function* fun, llvm::BasicBlock* successBlock, llvm::BasicBlock* failBlock, llvm::Value* value, llvm::Value* vtable, llvm::Value* rtstructinterface, llvm::Value* typeArgsPtr, llvm::Value* castTypeSubsts, bool useCastSiteID, bool guaranteedNoTypeArgs, bool guaranteedNoRawInvoke)
		{
			unsigned int extractIndex[2];
			extractIndex[0] = 0;
			extractIndex[1] = 1;
			auto typeUniqueFun = TypeRegistry::Instance().GetLLVMElement(*fun->getParent());
			BasicBlock* multiCastBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "multicasting structures not implemented!"); //BasicBlock::Create(LLVMCONTEXT, "multiCastCheck", fun);
			llvm::Value* specializedVtable = nullptr;
			if (!guaranteedNoTypeArgs) //still possibly no type args
			{

				BasicBlock* checkSpecializedVTableBlock = nullptr;
				BasicBlock* checkFreeSlotsBlock = nullptr;

				BasicBlock* checkNoTypeArgsBlock = builder->GetInsertBlock();
				BasicBlock* checkTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgs", fun);
				BasicBlock* checkTypeArgsLoopBlock = BasicBlock::Create(LLVMCONTEXT, "checkTypeArgsLoop", fun);
				BasicBlock* copyVTableBlock = BasicBlock::Create(LLVMCONTEXT, "copyVTable", fun);

				BasicBlock** firstRegularBlock = PreferTypeArgumentsInVTables ? &checkSpecializedVTableBlock : &checkFreeSlotsBlock;
				BasicBlock** secondRegularBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &checkSpecializedVTableBlock;

				BasicBlock** checkSpecializeVTableFailBlock = PreferTypeArgumentsInVTables ? &checkFreeSlotsBlock : &copyVTableBlock;
				BasicBlock** checkFreeSlotsFailBlock = PreferTypeArgumentsInVTables ? &copyVTableBlock : &checkSpecializedVTableBlock;

				BasicBlock* castSiteMatch = BasicBlock::Create(LLVMCONTEXT, "castSiteMatch", fun);
				BasicBlock* writeOptimizedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "justWriteInVtable", fun);

				*firstRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkSpecializedVTable" : "checkFreeSlots", fun);
				*secondRegularBlock = BasicBlock::Create(LLVMCONTEXT, PreferTypeArgumentsInVTables ? "checkFreeSlots" : "checkSpecializedVTable", fun);

				llvm::Value* rtstruct = nullptr;

				llvm::ConstantInt* castID = nullptr;
				//check cast site match (only for fully statically known types without variables)
				if (useCastSiteID)
				{
					checkNoTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "checkNoTypeArgs", fun);
					rtstruct = StructHeader::GenerateReadStructDescriptor(builder, value);
					auto specializedCastID = RTStruct::GenerateReadSpecializedVTableCastID(builder, rtstruct);
					castID = MakeInt(RTCast::nextCastSiteID());
					auto specializedTypeMatch = builder->CreateICmpEQ(specializedCastID, castID, "castSiteIsMatch");

					builder->CreateCondBr(specializedTypeMatch, castSiteMatch, checkNoTypeArgsBlock);
				}

				builder->SetInsertPoint(checkNoTypeArgsBlock);
				llvm::Value* targcount = RTInterface::GenerateReadTypeArgCount(builder, rtstructinterface);
				auto noTypeArgs = builder->CreateICmpEQ(targcount, MakeIntLike(targcount, 0), "typeArgCountIsZero");
				builder->CreateCondBr(noTypeArgs, writeOptimizedVTableBlock, *firstRegularBlock);

				// check specialized vtable
				{
					builder->SetInsertPoint(checkSpecializedVTableBlock);
					BasicBlock* specializedVTableExistsBlock = BasicBlock::Create(LLVMCONTEXT, "specializedVTableExists", fun);
					if (rtstruct == nullptr)
					{
						rtstruct = StructHeader::GenerateReadStructDescriptor(builder, value);
					}
					specializedVtable = RTStruct::GenerateReadSpecializedVTable(builder, rtstruct);
					builder->CreateCondBr(builder->CreateIsNotNull(specializedVtable), specializedVTableExistsBlock, *checkSpecializeVTableFailBlock);

					builder->SetInsertPoint(specializedVTableExistsBlock);
					auto origInterface = RTStructInterface::GetOriginalInterface(builder, specializedVtable);
					auto interfacesMatch = CreatePointerEq(builder, origInterface, rtstructinterface);
					auto negTypeArgCount = builder->CreateNeg(targcount);
					auto castTypeSubstsTypes = MakeLoad(builder, castTypeSubsts, MakeInt32(TypeArgumentListStackFields::Types));
					auto pointerEqCheckBlock = builder->GetInsertBlock();
					builder->CreateCondBr(interfacesMatch, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

					builder->SetInsertPoint(checkTypeArgsBlock);
					PHINode* argPosPHI = builder->CreatePHI(inttype(32), 2, "typeArgPos");
					argPosPHI->addIncoming(negTypeArgCount, pointerEqCheckBlock);
					auto typeArgsFinished = builder->CreateICmpEQ(argPosPHI, MakeInt32(0), "typeArgsFinished");
					builder->CreateCondBr(typeArgsFinished, checkTypeArgsLoopBlock, castSiteMatch);

					builder->SetInsertPoint(checkTypeArgsLoopBlock);
					auto typeEqFun = RTTypeEq::Instance().GetLLVMElement(*fun->getParent());
					auto leftTypeArg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, argPosPHI), "leftTypeArg");
					auto rightTypeArg = RTStructInterface::GetTypeArgumentNegativeIndex(builder, specializedVtable, argPosPHI);
					auto typeEqResult = builder->CreateCall(typeEqFun, { leftTypeArg, castTypeSubstsTypes, rightTypeArg });
					typeEqResult->setCallingConv(typeEqFun->getCallingConv());
					argPosPHI->addIncoming(builder->CreateAdd(argPosPHI, MakeInt32(1)), builder->GetInsertBlock());
					builder->CreateCondBr(typeEqResult, checkTypeArgsBlock, *checkSpecializeVTableFailBlock);

				}

				//check free slots (at this point we know that targcount>0)
				{
					builder->SetInsertPoint(checkFreeSlotsBlock);
					BasicBlock* enoughFreeSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "enoughFreeTypeArgSlots", fun);
					BasicBlock* copyOtherTypeArgumentsLoopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Head", fun);
					BasicBlock* copyOtherTypeArgumentsLoopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "copyOtherTypeArgumentsLoop$Body", fun);
					auto freeSlots = RefValueHeader::GenerateGetReserveTypeArgsFromVTablePointer(builder, vtable);
					auto enoughFreeSlots = builder->CreateICmpULE(targcount, freeSlots, "enoughFreeSlots");
					builder->CreateCondBr(enoughFreeSlots, enoughFreeSlotsBlock, *checkFreeSlotsFailBlock);

					builder->SetInsertPoint(enoughFreeSlotsBlock);
					if (NomCastStats)
					{
						builder->CreateCall(GetIncEnoughSpaceCastsFunction(*builder->GetInsertBlock()->getParent()->getParent()), {});
					}
					auto firstIndex = MakeInt32(-1);
					auto firstTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, firstIndex));
					auto firstUniquedTarg = builder->CreateCall(typeUniqueFun, { firstTarg, castTypeSubsts });
					firstUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					auto cmpxchgresult = StructHeader::GenerateWriteCastTypeArgumentCMPXCHG(builder, value, MakeInt32(0), firstUniquedTarg);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, copyOtherTypeArgumentsLoopHeadBlock, multiCastBlock);

					builder->SetInsertPoint(copyOtherTypeArgumentsLoopHeadBlock);
					auto moreThanOneTypeArg = builder->CreateICmpUGT(targcount, MakeIntLike(targcount, 1));
					builder->CreateCondBr(moreThanOneTypeArg, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);

					builder->SetInsertPoint(copyOtherTypeArgumentsLoopBodyBlock);
					auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
					targposPHI->addIncoming(MakeInt32(1), copyOtherTypeArgumentsLoopHeadBlock);
					auto nextTargPos = builder->CreateAdd(targposPHI, MakeInt32(1));
					targposPHI->addIncoming(nextTargPos, copyOtherTypeArgumentsLoopBodyBlock);

					auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, builder->CreateSub(MakeInt32(-1), targposPHI)));
					auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, castTypeSubsts });
					currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					StructHeader::GenerateWriteCastTypeArgument(builder, value, targposPHI, currentUniquedTarg); //no cmpxchgs necessary here, as the first one serves as lock

					auto hasMoreTypeArgs = builder->CreateICmpUGT(targcount, nextTargPos);
					builder->CreateCondBr(hasMoreTypeArgs, copyOtherTypeArgumentsLoopBodyBlock, writeOptimizedVTableBlock);
				}

				//copy vtable
				{
					BasicBlock* copyTypeArgsBlock = BasicBlock::Create(LLVMCONTEXT, "copyTypeArgsToVtable", fun);
					BasicBlock* writeVtableBlock = BasicBlock::Create(LLVMCONTEXT, "writeCopiedVtablePtr", fun);
					BasicBlock* adjustStructDataBlock = BasicBlock::Create(LLVMCONTEXT, "adjustStructData", fun);
					BasicBlock* tryWriteSpecializedVTableBlock = BasicBlock::Create(LLVMCONTEXT, "tryWriteSpecializedVTable", fun);
					BasicBlock* checkEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "checkEnoughPreallocatedSlots", fun);
					BasicBlock* notEnoughPreallocatedSlotsBlock = BasicBlock::Create(LLVMCONTEXT, "notEnoughPreallocatedSlots", fun);
					builder->SetInsertPoint(copyVTableBlock);
					auto newVtable = RTStructInterface::CreateCopyVTable(builder, rtstructinterface);
					auto typeArgsTarget = RTStructInterface::GetTypeArgumentsPointer(builder, newVtable);
					copyVTableBlock = builder->GetInsertBlock();
					builder->CreateBr(copyTypeArgsBlock);

					builder->SetInsertPoint(copyTypeArgsBlock);
					auto targposPHI = builder->CreatePHI(numtype(int32_t), 2, "typeArgWritePos");
					targposPHI->addIncoming(MakeInt32(-1), copyVTableBlock);

					auto currentTarg = MakeLoad(builder, builder->CreateGEP(typeArgsPtr, targposPHI));
					auto currentUniquedTarg = builder->CreateCall(typeUniqueFun, { currentTarg, castTypeSubsts });
					currentUniquedTarg->setCallingConv(typeUniqueFun->getCallingConv());
					MakeStore(builder, currentUniquedTarg, builder->CreatePointerCast(newVtable, RTStructInterface::GetLLVMType()->getPointerTo()), { MakeInt32(RTStructInterfaceFields::TypeArgs), targposPHI });

					auto nextTargPos = builder->CreateSub(targposPHI, MakeInt32(1));
					targposPHI->addIncoming(nextTargPos, builder->GetInsertBlock());
					auto targsDone = builder->CreateICmpEQ(targposPHI, builder->CreateNeg(targcount));
					builder->CreateCondBr(targsDone, writeVtableBlock, copyTypeArgsBlock);

					builder->SetInsertPoint(writeVtableBlock);
					auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, newVtable, vtable);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, adjustStructDataBlock, multiCastBlock);

					builder->SetInsertPoint(adjustStructDataBlock);
					auto slotsAddress = builder->CreateGEP(builder->CreatePointerCast(rtstruct, RTStruct::GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTStructFields::PreallocatedSlots) });
					auto typeArgSlotsCount = MakeLoad(builder, slotsAddress);
					auto extTargCount = builder->CreateZExtOrTrunc(targcount, typeArgSlotsCount->getType());
					auto specializedVtableIsNull = builder->CreateIsNull(specializedVtable, "specializedVTableIsNull");
					builder->CreateCondBr(specializedVtableIsNull, tryWriteSpecializedVTableBlock, checkEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(tryWriteSpecializedVTableBlock);
					RTStruct::GenerateWriteSpecializedVTable(builder, rtstruct, castID, newVtable);
					tryWriteSpecializedVTableBlock = builder->GetInsertBlock();
					builder->CreateBr(checkEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(checkEnoughPreallocatedSlotsBlock);
					auto currentPreAllocatedSlotsCount = builder->CreatePHI(typeArgSlotsCount->getType(), 3);
					currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, tryWriteSpecializedVTableBlock);
					currentPreAllocatedSlotsCount->addIncoming(typeArgSlotsCount, adjustStructDataBlock);
					auto enoughPreallocatedSlots = builder->CreateICmpULE(extTargCount, currentPreAllocatedSlotsCount, "enoughPreallocatedSlots");
					builder->CreateCondBr(enoughPreallocatedSlots, successBlock, notEnoughPreallocatedSlotsBlock);

					builder->SetInsertPoint(notEnoughPreallocatedSlotsBlock);
					auto updatePreallocatedSlotsResult = builder->CreateAtomicCmpXchg(slotsAddress, typeArgSlotsCount, extTargCount, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
					currentPreAllocatedSlotsCount->addIncoming(builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex, 1)), builder->GetInsertBlock());
					auto updateSuccess = builder->CreateExtractValue(updatePreallocatedSlotsResult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(updateSuccess, successBlock, checkEnoughPreallocatedSlotsBlock);
				}

				// cast site match
				{
					builder->SetInsertPoint(castSiteMatch);

					if (!guaranteedNoRawInvoke)
					{
						//todo: handle raw invoke wrappers
					}

					specializedVtable = RTStruct::GenerateReadSpecializedVTable(builder, rtstruct);
					auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
					auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
					builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
				}

				builder->SetInsertPoint(writeOptimizedVTableBlock);
			}

			if (!guaranteedNoRawInvoke)
			{
				//todo: handle raw invoke wrappers
			}
			specializedVtable = RTGeneralInterface::GenerateReadOptimizedVTable(builder, rtstructinterface);
			auto cmpxchgresult = RefValueHeader::GenerateWriteVTablePointerCMPXCHG(builder, value, specializedVtable, vtable);
			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
			builder->CreateCondBr(cmpxchgsuccess, successBlock, multiCastBlock);
		}

	

		llvm::FunctionType* RTCast::GetAdjustFunctionType()
		{
			static auto ft = FunctionType::get(BOOLTYPE, ArrayRef<Type*>({ REFTYPE, TYPETYPE }), false);
			return ft;
		}

		llvm::Value* GenerateEnvSubstitutions(NomBuilder& builder, CompileEnv* env, NomTypeRef type = nullptr)
		{
			auto targcount = env->GetEnvTypeArgumentCount() + env->GetLocalTypeArgumentCount();
			if (targcount > 0 && (type == nullptr || type->ContainsVariables()))
			{
				auto talst = builder->CreateAlloca(RTSubtyping::TypeArgumentListStackType(), MakeInt32(1), "envSubsts");
				MakeStore(builder, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), talst, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Next));
				if (env->GetLocalTypeArgumentCount() > 0)
				{
					auto targarr = builder->CreateAlloca(TYPETYPE, MakeInt(targcount), "targarr");
					for (int32_t i = 0; i < targcount; i++)
					{
						MakeStore(builder, env->GetTypeArgument(builder, i), builder->CreateGEP(targarr, MakeInt32(targcount - (i + 1))));
					}
					MakeStore(builder, builder->CreateGEP(targarr, MakeInt32(targcount)), talst, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
				}
				else
				{
					MakeStore(builder, env->GetEnvTypeArgumentArray(builder), talst, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
				}
				return talst;
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

			SmallVector<tuple<BasicBlock*, Value*, Value*, Function*>, 8> targsInObjectSources;
			SmallVector<pair<BasicBlock*, Function*>, 4> targsInVTableSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);

				BasicBlock* vtableBlock = nullptr, * lambdaBlock = nullptr, * structBlock = nullptr, * partialAppBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateVTableTagSwitch(builder, value, &vtableVar, &vtableBlock, &lambdaBlock, &structBlock, &partialAppBlock);

				//Already casted objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (vtableBlock != nullptr)
				{
					BasicBlock* classObjectBlock = nullptr, * lambda_targsInObject_Block = nullptr, * lambda_targsInVTable_Block = nullptr, * struct_targsInObject_Block = nullptr, * struct_targsInVTable_Block = nullptr, * partialAppBlock = nullptr, * multiCastBlock = nullptr;
					builder->SetInsertPoint(vtableBlock);
					int vtableKindCases = RTVTable::GenerateVTableKindSwitch(builder, vtableVar, &classObjectBlock, &lambda_targsInObject_Block, &lambda_targsInVTable_Block, &struct_targsInObject_Block, &struct_targsInVTable_Block, &partialAppBlock, &multiCastBlock);

					if (classObjectBlock != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(classObjectBlock, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (lambda_targsInObject_Block != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(lambda_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (struct_targsInObject_Block != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(struct_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (lambda_targsInVTable_Block != nullptr)
					{
						targsInVTableSources.push_back(make_pair(lambda_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (struct_targsInVTable_Block != nullptr)
					{
						targsInVTableSources.push_back(make_pair(struct_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (partialAppBlock != nullptr)
					{
						targsInVTableSources.push_back(make_pair(partialAppBlock, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (multiCastBlock != nullptr)
					{
						RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting of multicasted objects not implemented!", multiCastBlock); //TODO: implement
					}
				}

				//PURE LAMBDA
				if (lambdaBlock != nullptr)
				{
					builder->SetInsertPoint(lambdaBlock);
					if (!type->Named->IsInterface())
					{
						builder->CreateBr(outFalseBlock);
					}
					else if (!type->Named->IsFunctional())
					{
						builder->CreateBr(outFalseBlock);
					}
					else
					{
						GenerateMonotonicLambdaCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, type->Named->GetLLVMElement(*fun->getParent()), GenerateEnvSubstitutions(builder, env, type), MakeInt32(type->Arguments.size()), RTClassType::GetTypeArgumentsPtr(builder, type->GetLLVMElement(*fun->getParent())), !type->ContainsVariables(), type->Named->GetTypeParametersCount() == 0);
					}
				}

				//PURE STRUCT
				if (structBlock != nullptr)
				{
					builder->SetInsertPoint(structBlock);
					if (!type->Named->IsInterface())
					{
						builder->CreateBr(outFalseBlock);
					}
					else
					{
						auto envSubsts = GenerateEnvSubstitutions(builder, env, type);
						GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, type->Named->GetLLVMElement(*fun->getParent()), RTClassType::GetTypeArgumentsPtr(builder, type->GetLLVMElement(*fun->getParent())), envSubsts, !type->ContainsVariables(), type->Named->GetTypeParametersCount() == 0, !type->Named->GetHasRawInvoke());
					}
				}

				//PARTIAL APPLICATION
				if (partialAppBlock != nullptr)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting partial applications not implemented", partialAppBlock); //TODO: implement
				}
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
					targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
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
					targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
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
					targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
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
					targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
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
					targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
				}
			}

			int nominalSubtypingCases = targsInObjectSources.size() + targsInVTableSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* adjustFunction = nullptr;
				PHINode* adjustPHI = nullptr;
				Value* ifacePtr = nullptr;
				PHINode* ifacePHI = nullptr;
				Value* typeArgsPtr = nullptr;
				PHINode* typeArgsPHI = nullptr;
				if (nominalSubtypingCases > 1)
				{
					builder->SetInsertPoint(nominalSubtypingCheck);
					adjustPHI = builder->CreatePHI(GetAdjustFunctionType()->getPointerTo(), nominalSubtypingCases, "adjustFunction");
					ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), nominalSubtypingCases, "interfacePointer");
					typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), nominalSubtypingCases, "typeArgs");
					adjustFunction = adjustPHI;
					ifacePtr = ifacePHI;
					typeArgsPtr = typeArgsPHI;
				}
				for (auto& tpl : targsInObjectSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(std::get<1>(tpl), TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(std::get<2>(tpl), RTInterface::GetLLVMType()->getPointerTo());
					auto adjustFun = std::get<3>(tpl);

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
						adjustPHI->addIncoming(adjustFun, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
						adjustFunction = adjustFun;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}
				for (auto& tpl : targsInVTableSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(vtableVar, TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(vtableVar, RTInterface::GetLLVMType()->getPointerTo());
					auto adjustFun = std::get<1>(tpl);

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
						adjustPHI->addIncoming(adjustFun, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
						adjustFunction = adjustFun;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}

				builder->SetInsertPoint(nominalSubtypingCheck);
				auto instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				auto envSubstitutions = GenerateEnvSubstitutions(builder, env, type);
				BasicBlock* stErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Error during subtyping check!");
				BasicBlock* adjustBlock = BasicBlock::Create(LLVMCONTEXT, "callAdjustFunction", fun);
				RTSubtyping::CreateInlineSubtypingCheck(builder, instanceType, type, envSubstitutions, outTrueBlock, adjustBlock, stErrorBlock);

				builder->SetInsertPoint(adjustBlock);
				auto adjustResult = builder->CreateCall(GetAdjustFunctionType(), adjustFunction, { *value, type->GetLLVMElement(*fun->getParent()) });
				adjustResult->setCallingConv(NOMCC);
				builder->CreateCondBr(adjustResult, outTrueBlock, outFalseBlock);
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

			SmallVector<tuple<BasicBlock*, Value*, Value*, Function*>, 8> targsInObjectSources;
			SmallVector<pair<BasicBlock*, Function*>, 4> targsInVTableSources;
			Value* vtableVar = nullptr;
			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);

				BasicBlock* vtableBlock = nullptr, * lambdaBlock = nullptr, * structBlock = nullptr, * partialAppBlock = nullptr;

				int vtableCases = RefValueHeader::GenerateVTableTagSwitch(builder, value, &vtableVar, &vtableBlock, &lambdaBlock, &structBlock, &partialAppBlock);

				//Already casted objects; just collecting blocks here for unified treatments with ints/floats/bools outside
				if (vtableBlock != nullptr)
				{
					BasicBlock* classObjectBlock = nullptr, * lambda_targsInObject_Block = nullptr, * lambda_targsInVTable_Block = nullptr, * struct_targsInObject_Block = nullptr, * struct_targsInVTable_Block = nullptr, * partialAppBlock = nullptr, * multiCastBlock = nullptr;
					builder->SetInsertPoint(vtableBlock);
					int vtableKindCases = RTVTable::GenerateVTableKindSwitch(builder, vtableVar, &classObjectBlock, &lambda_targsInObject_Block, &lambda_targsInVTable_Block, &struct_targsInObject_Block, &struct_targsInVTable_Block, &partialAppBlock, &multiCastBlock);

					if (classObjectBlock != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(classObjectBlock, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (lambda_targsInObject_Block != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(lambda_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (struct_targsInObject_Block != nullptr)
					{
						targsInObjectSources.push_back(make_tuple(struct_targsInObject_Block, *value, vtableVar, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (lambda_targsInVTable_Block != nullptr)
					{
						targsInVTableSources.push_back(make_pair(lambda_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (struct_targsInVTable_Block != nullptr)
					{
						targsInVTableSources.push_back(make_pair(struct_targsInVTable_Block, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (partialAppBlock != nullptr)
					{
						targsInVTableSources.push_back(make_pair(partialAppBlock, FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
					}
					if (multiCastBlock != nullptr)
					{
						RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting of multicasted objects not implemented!", multiCastBlock); //TODO: implement
					}
				}

				//PURE LAMBDA - note difference to inlined version above: there we know we have a class type, here we can get any type
				if (lambdaBlock != nullptr)
				{
					BasicBlock* rightClassTypeBlock = nullptr, * rightTypeVarBlock = nullptr, * rightInstanceTypeBlock = nullptr, * rightMaybeTypeBlock = nullptr;
					builder->SetInsertPoint(lambdaBlock);
					PHINode* rightPHI = builder->CreatePHI(type->getType(), 3, "rightType");
					rightPHI->addIncoming(type, refValueBlock);
					RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, &rightClassTypeBlock, &outTrueBlock, &rightTypeVarBlock, &outFalseBlock, &rightInstanceTypeBlock, &outFalseBlock, &outFalseBlock, &outTrueBlock, &rightMaybeTypeBlock);

					if (rightTypeVarBlock != nullptr)
					{
						builder->SetInsertPoint(rightTypeVarBlock);
						auto envSubsts = GenerateEnvSubstitutions(builder, env);
						auto typeList = MakeLoad(builder, envSubsts, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
						auto newRightType = MakeLoad(builder, builder->CreateGEP(typeList, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
						rightPHI->addIncoming(newRightType, builder->GetInsertBlock()); //this type should not contain any more type variables, so the code here getting the variables from the environment again should be safe
						builder->CreateBr(lambdaBlock);
					}

					if (rightMaybeTypeBlock != nullptr)
					{
						builder->SetInsertPoint(rightMaybeTypeBlock);
						auto newRightType = RTMaybeType::GenerateReadPotentialType(builder, rightPHI);
						rightPHI->addIncoming(newRightType, builder->GetInsertBlock());
						builder->CreateBr(lambdaBlock);
					}

					if (rightClassTypeBlock)
					{
						BasicBlock* lambdaCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureLambda$classType", fun);
						builder->SetInsertPoint(rightClassTypeBlock);
						auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI);
						auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						auto isFunctionalInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
						builder->CreateCondBr(isFunctionalInterface, lambdaCastBlock, outFalseBlock);

						builder->SetInsertPoint(lambdaCastBlock);
						GenerateMonotonicLambdaCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, iface, GenerateEnvSubstitutions(builder, env), RTInterface::GenerateReadTypeArgCount(builder, iface), RTClassType::GetTypeArgumentsPtr(builder, rightPHI), false, false);
					}

					if (rightInstanceTypeBlock)
					{
						BasicBlock* lambdaCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureLambda$instanceType", fun);
						builder->SetInsertPoint(rightInstanceTypeBlock);
						auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI);
						auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						auto isFunctionalInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsFunctional | RTInterfaceFlags::IsInterface));
						builder->CreateCondBr(isFunctionalInterface, lambdaCastBlock, outFalseBlock);

						builder->SetInsertPoint(lambdaCastBlock);
						GenerateMonotonicLambdaCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, iface, GenerateEnvSubstitutions(builder, env), RTInterface::GenerateReadTypeArgCount(builder, iface), RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), false, false);
					}
				}

				//PURE STRUCT - note difference to inlined version above: there we know we have a class type, here we can get any type
				if (structBlock != nullptr)
				{
					BasicBlock* rightClassTypeBlock = nullptr, * rightTypeVarBlock = nullptr, * rightInstanceTypeBlock = nullptr, * rightMaybeTypeBlock = nullptr;
					builder->SetInsertPoint(structBlock);
					PHINode* rightPHI = builder->CreatePHI(type->getType(), 3, "rightType");
					rightPHI->addIncoming(type, refValueBlock);
					RTTypeHead::GenerateTypeKindSwitch(builder, rightPHI, &rightClassTypeBlock, &outTrueBlock, &rightTypeVarBlock, &outFalseBlock, &rightInstanceTypeBlock, &outFalseBlock, &outFalseBlock, &outTrueBlock, &rightMaybeTypeBlock);

					if (rightTypeVarBlock != nullptr)
					{
						builder->SetInsertPoint(rightTypeVarBlock);
						auto envSubsts = GenerateEnvSubstitutions(builder, env);
						auto typeList = MakeLoad(builder, envSubsts, MakeInt<TypeArgumentListStackFields>(TypeArgumentListStackFields::Types));
						auto newRightType = MakeLoad(builder, builder->CreateGEP(typeList, builder->CreateSub(MakeInt32(-1), RTTypeVar::GenerateLoadIndex(builder, rightPHI))));
						rightPHI->addIncoming(newRightType, builder->GetInsertBlock()); //this type should not contain any more type variables, so the code here getting the variables from the environment again should be safe
						builder->CreateBr(structBlock);
					}

					if (rightMaybeTypeBlock != nullptr)
					{
						builder->SetInsertPoint(rightMaybeTypeBlock);
						auto newRightType = RTMaybeType::GenerateReadPotentialType(builder, rightPHI);
						rightPHI->addIncoming(newRightType, builder->GetInsertBlock());
						builder->CreateBr(structBlock);
					}

					if (rightClassTypeBlock)
					{
						BasicBlock* structCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureStruct$classType", fun);
						builder->SetInsertPoint(rightClassTypeBlock);
						auto iface = RTClassType::GenerateReadClassDescriptorLink(builder, rightPHI);
						auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						auto isInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface));
						builder->CreateCondBr(isInterface, structCastBlock, outFalseBlock);

						builder->SetInsertPoint(structCastBlock);
						auto envSubsts = GenerateEnvSubstitutions(builder, env);
						GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, iface, RTClassType::GetTypeArgumentsPtr(builder, rightPHI), envSubsts, false, false, false);
					}

					if (rightInstanceTypeBlock)
					{
						BasicBlock* structCastBlock = BasicBlock::Create(LLVMCONTEXT, "castPureStruct$instanceType", fun);
						builder->SetInsertPoint(rightInstanceTypeBlock);
						auto iface = RTInstanceType::GenerateReadClassDescriptorLink(builder, rightPHI);
						auto ifaceFlags = RTInterface::GenerateReadFlags(builder, iface);
						auto isInterface = builder->CreateICmpEQ(builder->CreateAnd(ifaceFlags, MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface)), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::IsInterface));
						builder->CreateCondBr(isInterface, structCastBlock, outFalseBlock);

						builder->SetInsertPoint(structCastBlock);
						auto envSubsts = GenerateEnvSubstitutions(builder, env);
						GenerateMonotonicStructCast(builder, env, fun, outTrueBlock, outFalseBlock, value, vtableVar, iface, RTInstanceType::GetTypeArgumentsPtr(builder, rightPHI), envSubsts, false, false, false);
					}
				}

				//PARTIAL APPLICATION
				if (partialAppBlock != nullptr)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Casting partial applications not implemented", partialAppBlock); //TODO: implement
				}
			}

			if (intBlock != nullptr)
			{
				builder->SetInsertPoint(intBlock);
				targsInObjectSources.push_back(make_tuple(intBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
			}

			if (floatBlock != nullptr)
			{
				builder->SetInsertPoint(floatBlock);
				targsInObjectSources.push_back(make_tuple(floatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
			}

			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				targsInObjectSources.push_back(make_tuple(primitiveIntBlock, *value, (Value*)ConstantExpr::getPointerCast(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
			}

			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				targsInObjectSources.push_back(make_tuple(primitiveFloatBlock, *value, (Value*)ConstantExpr::getPointerCast(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
			}

			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				targsInObjectSources.push_back(make_tuple(primitiveBoolBlock, *value, (Value*)ConstantExpr::getPointerCast(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), FailingAdjustFun::GetInstance()->GetLLVMElement(*fun->getParent())));
			}

			int nominalSubtypingCases = targsInObjectSources.size() + targsInVTableSources.size();
			if (nominalSubtypingCases > 0)
			{
				BasicBlock* nominalSubtypingCheck = BasicBlock::Create(LLVMCONTEXT, "nominalSubtypingCheck", fun);
				Value* adjustFunction = nullptr;
				PHINode* adjustPHI = nullptr;
				Value* ifacePtr = nullptr;
				PHINode* ifacePHI = nullptr;
				Value* typeArgsPtr = nullptr;
				PHINode* typeArgsPHI = nullptr;
				if (nominalSubtypingCases > 1)
				{
					builder->SetInsertPoint(nominalSubtypingCheck);
					adjustPHI = builder->CreatePHI(GetAdjustFunctionType()->getPointerTo(), nominalSubtypingCases, "adjustFunction");
					ifacePHI = builder->CreatePHI(RTInterface::GetLLVMType()->getPointerTo(), nominalSubtypingCases, "interfacePointer");
					typeArgsPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), nominalSubtypingCases, "typeArgs");
					adjustFunction = adjustPHI;
					ifacePtr = ifacePHI;
					typeArgsPtr = typeArgsPHI;
				}
				for (auto& tpl : targsInObjectSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(std::get<1>(tpl), TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(std::get<2>(tpl), RTInterface::GetLLVMType()->getPointerTo());
					auto adjustFun = std::get<3>(tpl);

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
						adjustPHI->addIncoming(adjustFun, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
						adjustFunction = adjustFun;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}
				for (auto& tpl : targsInVTableSources)
				{
					builder->SetInsertPoint(std::get<0>(tpl));
					auto typeArgsCast = builder->CreatePointerCast(vtableVar, TYPETYPE->getPointerTo());
					auto ifaceCast = builder->CreatePointerCast(vtableVar, RTInterface::GetLLVMType()->getPointerTo());
					auto adjustFun = std::get<1>(tpl);

					if (nominalSubtypingCases > 1)
					{
						typeArgsPHI->addIncoming(typeArgsCast, builder->GetInsertBlock());
						ifacePHI->addIncoming(ifaceCast, builder->GetInsertBlock());
						adjustPHI->addIncoming(adjustFun, builder->GetInsertBlock());
					}
					else
					{
						typeArgsPtr = typeArgsCast;
						ifacePtr = ifaceCast;
						adjustFunction = adjustFun;
					}
					builder->CreateBr(nominalSubtypingCheck);
				}

				builder->SetInsertPoint(nominalSubtypingCheck);
				Value* instanceType = builder->CreateAlloca(RTInstanceType::GetLLVMType(), MakeInt32(1));
				RTInstanceType::CreateInitialization(builder, *fun->getParent(), instanceType, MakeInt<size_t>(0), ConstantPointerNull::get(POINTERTYPE), ifacePtr, typeArgsPtr);
				auto envSubstitutions = GenerateEnvSubstitutions(builder, env);
				BasicBlock* stErrorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Error during subtyping check!");
				BasicBlock* adjustBlock = BasicBlock::Create(LLVMCONTEXT, "callAdjustFunction", fun);
				instanceType = builder->CreateGEP(instanceType, { MakeInt32(0), MakeInt32(RTInstanceTypeFields::Head) });
				auto subtypingResult = builder->CreateCall(RTSubtyping::Instance().GetLLVMElement(*fun->getParent()), { instanceType, type, ConstantPointerNull::get(RTSubtyping::TypeArgumentListStackType()->getPointerTo()), envSubstitutions });
				builder->CreateCondBr(builder->CreateICmpEQ(subtypingResult, MakeUInt(2, 0)), adjustBlock, outTrueBlock);

				builder->SetInsertPoint(adjustBlock);
				auto adjustResult = builder->CreateCall(GetAdjustFunctionType(), adjustFunction, { *value, type });
				adjustResult->setCallingConv(NOMCC);
				builder->CreateCondBr(adjustResult, outTrueBlock, outFalseBlock);
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
		llvm::Value* GenerateGetTypeArgumentListStackTypes(NomBuilder& builder, llvm::Value* talst)
		{
			if (talst->getValueID() == Value::ConstantPointerNullVal)
			{
				return ConstantPointerNull::get(TYPETYPE->getPointerTo());
			}
			BasicBlock* origBlock = builder->GetInsertBlock();
			BasicBlock* isNotNull = BasicBlock::Create(LLVMCONTEXT, "talstNotNULL", origBlock->getParent());
			BasicBlock* merge = BasicBlock::Create(LLVMCONTEXT, "talstMerge", origBlock->getParent());
			builder->CreateCondBr(builder->CreateIsNotNull(talst), isNotNull, merge);

			builder->SetInsertPoint(merge);
			PHINode* mergePHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2);
			mergePHI->addIncoming(ConstantPointerNull::get(TYPETYPE->getPointerTo()), origBlock);

			builder->SetInsertPoint(isNotNull);
			auto typeArr = MakeLoad(builder, talst, MakeInt32(TypeArgumentListStackFields::Types));
			mergePHI->addIncoming(typeArr, builder->GetInsertBlock());
			builder->CreateBr(merge);

			builder->SetInsertPoint(merge);
			return mergePHI;
		}
	}
}
