#include "LambdaHeader.h"
#include "RTVTable.h"
#include "RefValueHeader.h"
#include "RTLambda.h"
#include "NomVMInterface.h"
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "NomLambda.h"
#include "NomTypeRegistry.h"
#include "RTCompileConfig.h"
#include "RTStructuralVTable.h"
#include "StructHeader.h"
#include "StructuralValueHeader.h"
#include "RTSTable.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* LambdaHeader::GetLLVMType()
		{
			static StructType* lhtype = StructType::create(LLVMCONTEXT, "RT_NOM_LambdaHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				lhtype->setBody(
					StructuralValueHeader::GetLLVMType(),
					//arrtype(TYPETYPE, 0),																	// closure type arguments
					////arrtype(TYPETYPE, 0),																	// optimized type arguments
					//RefValueHeader::GetLLVMType(),															// vtable or just tag info
					//(NomLambdaOptimizationLevel > 0 ? (llvm::Type*)POINTERTYPE : arrtype(POINTERTYPE, 0)),	// potential space for raw invoke pointer
					//RTLambda::GetLLVMType()->getPointerTo(),												// Metadata pointer
					//POINTERTYPE,																			// null for uncasted lambdas, pointer to type for singly-casted lambdas, pointer to type list with LSB 1 for multi-casted lambdas
					arrtype(REFTYPE, 0));																	// closure fields
			}
			return lhtype;
		}

		llvm::Value* LambdaHeader::GenerateReadCastTypePointer(NomBuilder& builder, llvm::Value* thisObj)
		{
			return StructuralValueHeader::GenerateReadCastData(builder, thisObj);
			//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastReference) }));
		}
		//llvm::Value* LambdaHeader::GenerateWriteCastTypePointerCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* newPtr, llvm::Value* oldPtr)
		//{
		//	auto argPtr = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastReference) });
		//	return builder->CreateAtomicCmpXchg(argPtr, oldPtr, newPtr, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		//}
		llvm::Value* LambdaHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldIndex)
		{
			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldIndex) }));
		}
		llvm::Value* LambdaHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldIndex, llvm::Value* value)
		{
			return MakeStore(builder, value, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldIndex) }));
		}
		llvm::Value* LambdaHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, const NomLambda* lambda)
		{
			return StructuralValueHeader::GenerateReadTypeArgument(builder, thisObj, MakeInt32(argindex));
//			return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::TypeArgs), /*builder->CreateSub(MakeInt32(lambda->GetArgumentCount() + lambda->GetTypeParametersCount()),*/ MakeInt32(-(argindex + 1)) }));
			//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(argindex), MakeSInt(32, (NomLambdaOptimizationLevel>0?1:0)+lambda->GetTypeParametersCount())) }));
		}
		//llvm::Value* LambdaHeader::GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* targ, const NomLambda* lambda)
		//{
		//	return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::TypeArgs), /*builder->CreateSub(MakeInt32(lambda->GetArgumentCount() + lambda->GetTypeParametersCount()), */MakeInt32(-(argindex + 1)) }));
		//	//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(argindex), MakeSInt(32, (NomLambdaOptimizationLevel>0?1:0)+lambda->GetTypeParametersCount())) }));
		//}
		llvm::Value* LambdaHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, const NomLambda *lambda)
		{
			return StructuralValueHeader::GenerateReadTypeArgsPtr(builder, thisObj);
			//return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::TypeArgs)/*, MakeInt32(lambda->GetArgumentCount() + lambda->GetTypeParametersCount())*/ });
		}
		//llvm::Value* LambdaHeader::GenerateReadCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex)
		//{
		//	return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastTypeArgs), MakeInt32(-(argindex+1)) }));
		//}
		//llvm::Value* LambdaHeader::GeneratePointerToCastTypeArguments(NomBuilder& builder, llvm::Value* thisObj)
		//{
		//	return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastTypeArgs), MakeInt32(0) });
		//}
		//llvm::Value* LambdaHeader::GenerateWriteCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		//{
		//	return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) }));
		//}
		//llvm::Value* LambdaHeader::GenerateWriteCastTypeArgumentCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		//{
		//	auto argPtr = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(LambdaHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) });
		//	return builder->CreateAtomicCmpXchg(argPtr, ConstantPointerNull::get(TYPETYPE), targ, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		//}
		llvm::Value* LambdaHeader::GenerateReadLambdaMetadata(NomBuilder& builder, llvm::Value* thisObj)
		{
			return builder->CreatePointerCast(StructuralValueHeader::GenerateReadSTablePtr(builder, thisObj), RTLambda::GetLLVMType()->getPointerTo());
			//auto loadInst = MakeLoad(builder, thisObj, GetLLVMType()->getPointerTo(), MakeInt32(LambdaHeaderFields::LambdaDescriptor), "lambdaDesc");
			//loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			//return builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(loadInst, numtype(intptr_t)), ConstantExpr::getXor(ConstantInt::getAllOnesValue(INTTYPE), MakeInt<intptr_t>(7))), loadInst->getType());
			////return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), MakeInt32(LambdaHeaderFields::LambdaDescriptor), "lambdaDesc");
		}
		void LambdaHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, llvm::ArrayRef<llvm::Value*> arguments, llvm::Constant* descriptorRef, const NomLambda *lambda)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetClosureAlloc(fun->getParent());
			llvm::Value* extraSlots = RTLambda::GenerateReadPreallocatedSlots(builder, descriptorRef);

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(arguments.size()+(NomLambdaOptimizationLevel>0?1:0)), MakeInt<size_t>(typeArguments.size()), extraSlots }), GetLLVMType()->getPointerTo());

			//int writePos = -(typeArguments.size() + arguments.size() + (NomLambdaOptimizationLevel > 0 ? 1 : 0));
			int fieldIndex=0;
			for (auto arg : arguments)
			{
				GenerateWriteField(builder, newmem, fieldIndex, EnsurePacked(builder, arg));
				//MakeStore(builder, EnsurePacked(builder, arg), newmem, { MakeInt32(LambdaHeaderFields::Fields), MakeInt32(writePos) });
				//writePos++;
				fieldIndex++;
			}
			int targIndex = 0;
			//for (auto targ : typeArguments)
			//{
			//	GenerateWriteTypeArgument(builder, newmem, targIndex, targ, lambda);
			//	//MakeStore(builder, targ, newmem, { MakeInt32(LambdaHeaderFields::TypeArgs), MakeInt32(writePos) });
			//	//writePos++;
			//	targIndex++;
			//}
			if (NomLambdaOptimizationLevel > 0)
			{
				RefValueHeader::GenerateWriteRawInvoke(builder, newmem, ConstantPointerNull::get(POINTERTYPE));
				//MakeStore(builder, ConstantPointerNull::get(REFTYPE), newmem, { MakeInt32(LambdaHeaderFields::RawInvokePointerSpace), MakeInt32(-1) });
			}
			auto vTablePtr = builder->CreatePointerCast(RTStructuralVTable::Instance().GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo());
			auto sTablePtr = builder->CreateIntToPtr(builder->CreateOr(ConstantExpr::getPtrToInt(descriptorRef, numtype(intptr_t)), MakeInt<intptr_t>((intptr_t)RTDescriptorKind::Lambda)), RTLambda::GetLLVMType()->getPointerTo());
			//MakeStore(builder, builder->CreatePointerCast(RTStructuralVTable::Instance().GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), newmem, { MakeInt32(LambdaHeaderFields::RefValueHeader), MakeInt32(RefValueHeaderFields::InterfaceTable) });
			//MakeStore(builder, builder->CreateIntToPtr(builder->CreateOr(ConstantExpr::getPtrToInt(descriptorRef, numtype(intptr_t)), MakeInt<intptr_t>((intptr_t)RTDescriptorKind::Lambda)), RTLambda::GetLLVMType()->getPointerTo()), newmem, MakeInt32(LambdaHeaderFields::LambdaDescriptor));

			//llvm::Value* instanceKind = MakeInt((unsigned char)RTDescriptorKind::Lambda);
			////instanceKind = builder->CreateAdd(instanceKind, builder->CreateZExtOrTrunc(builder->CreateICmpUGT(extraSlots, ConstantInt::get(extraSlots->getType(), 0)), instanceKind->getType()));

			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, sTablePtr);

			auto refValHeader = builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(LambdaHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			//auto argSlotCount = builder->CreateIntToPtr(builder->CreateShl(EnsureIntegerSize(builder, extraSlots, bitsin(intptr_t)), MakeInt<intptr_t>(bitsin(intptr_t) / 2)), POINTERTYPE);
			////auto argSlotCount = ConstantPointerNull::get(POINTERTYPE);

			//RefValueHeader::GenerateInitializerCode(builder, refValHeader, argSlotCount, instanceKind);
			builder->CreateRet(refValHeader);
		}
		llvm::Value* LambdaHeader::GenerateReadField(NomBuilder& builder, NomLambda* lambda, llvm::Value* thisObj, int32_t fieldindex)
		{
			//return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(LambdaHeaderFields::Fields), builder->CreateAdd(builder->CreateSub(MakeInt32(NomLambdaOptimizationLevel > 0 ? -1 : 0), MakeInt32((uint32_t)(lambda->GetTypeParametersCount() + lambda->GetArgumentCount()))), MakeInt32(fieldindex)) });
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldindex) });
		}
	}
}