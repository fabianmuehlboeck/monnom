#include "StructHeader.h"
#include "RefValueHeader.h"
#include "RTStruct.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"
#include "NomStruct.h"
#include "NomField.h"
#include "NomDynamicType.h"
#include "RTCompileConfig.h"
#include "RTStructuralVTable.h"
#include "RTVTable.h"
#include "RTOutput.h"
#include "Metadata.h"
#include "StructuralValueHeader.h"
#include "RTSTable.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* StructHeader::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					StructuralValueHeader::GetLLVMType(),													//common structural value stuff
					//arrtype(TYPETYPE, 0),																	//Closure type args
					////arrtype(TYPETYPE, 0),																	//cast type args
					//RefValueHeader::GetLLVMType(),															//vtable/flag
					//(NomLambdaOptimizationLevel > 0 ? (llvm::Type*)POINTERTYPE : arrtype(POINTERTYPE, 0)),	//potential space for raw invoke pointer
					//RTStruct::GetLLVMType()->getPointerTo(),												//struct metadata
					arrtype(llvm::Type::getInt8Ty(LLVMCONTEXT), GetConcurrentDictionarySize()),				//instance dictionary
					arrtype(REFTYPE, 0)																		//Fields
				);
			}
			return shst;
		}

		/// <summary>
		/// clears three least significant bits (tag)
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="objPointer"></param>
		/// <returns></returns>
		llvm::Value* StructHeader::GenerateReadStructDescriptor(NomBuilder& builder, llvm::Value* objPointer)
		{
			return StructuralValueHeader::GenerateReadSTablePtr(builder, objPointer);
			//auto loadInst = MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::StructDescriptor), "structDescriptor");
			//loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			//return builder->CreateIntToPtr(builder->CreateAnd(builder->CreatePtrToInt(loadInst, numtype(intptr_t)), ConstantExpr::getXor(ConstantInt::getAllOnesValue(INTTYPE), MakeInt<intptr_t>(7))), loadInst->getType());
		}
		/// <summary>
		/// Includes tag
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="objPointer"></param>
		/// <returns></returns>
		llvm::Value* StructHeader::GenerateReadStructDescriptorPtr(NomBuilder& builder, llvm::Value* objPointer)
		{
			return StructuralValueHeader::GenerateReadRawSTablePtr(builder, objPointer);
			//auto loadInst = MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::StructDescriptor), "structDescriptor");
			//loadInst->setMetadata("invariant.group", getStructDescriptorInvariantNode());
			//return loadInst;
		}

		llvm::Value* StructHeader::GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer)
		{
			return builder->CreateGEP(builder->CreatePointerCast(objPointer, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0) });
			//return MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::InstanceDictionary), "structInstanceDictionary");
		}

		/// <summary>
		/// Locks the value of a given (reference) value by making its LSB tag 0x4. This is only a valid operation if the field is already locked this way,
		/// or if the LSB tag is 0x0 (i.e. primitives cannot be locked). If the value is already locked, returns a null pointer, otherwise the (untagged)
		/// value of the now locked field.
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="thisObj"></param>
		/// <param name="fieldindex"></param>
		/// <param name="hasRawInvoke"></param>
		/// <returns></returns>
		llvm::Value* StructHeader::GenerateReadAndLockField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke)
		{
			static unsigned int extractIndex[2] = { 0,1 };
			static const char* lockErrorMsg = "Cannot lock primitive value!";
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "fieldLocked", builder->GetInsertBlock()->getParent());
			BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "lockLoop$head", builder->GetInsertBlock()->getParent());
			BasicBlock* loopRecurseBlock = BasicBlock::Create(LLVMCONTEXT, "lockLoop$recurse", builder->GetInsertBlock()->getParent());
			BasicBlock* tryLockBlock = BasicBlock::Create(LLVMCONTEXT, "tryLockField", builder->GetInsertBlock()->getParent());
			BasicBlock* checkPrimtiveBlock = BasicBlock::Create(LLVMCONTEXT, "checkPrimitive", builder->GetInsertBlock()->getParent());
			BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, lockErrorMsg);
			auto fieldAddress = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
			auto currentValue = MakeLoad(builder, fieldAddress);
			BasicBlock* incomingBlock = builder->GetInsertBlock();
			builder->CreateBr(loopHeadBlock);

			builder->SetInsertPoint(loopHeadBlock);
			auto currentValuePHI = builder->CreatePHI(currentValue->getType(), 2);
			currentValuePHI->addIncoming(currentValue, incomingBlock);
			auto currentValueAsInt = builder->CreatePtrToInt(currentValuePHI, numtype(intptr_t));
			auto currentValueTag = builder->CreateTrunc(currentValueAsInt, inttype(3));
			builder->CreateCondBr(builder->CreateICmpEQ(currentValueTag, MakeUInt(3, 0)), tryLockBlock, checkPrimtiveBlock);

			builder->SetInsertPoint(outBlock);
			auto outPHI = builder->CreatePHI(currentValuePHI->getType(), 2, "lockedFieldValue");

			builder->SetInsertPoint(checkPrimtiveBlock);
			builder->CreateCondBr(builder->CreateICmpEQ(currentValueTag, MakeUInt(3, 4)), outBlock, errorBlock);
			outPHI->addIncoming(ConstantPointerNull::get(REFTYPE), checkPrimtiveBlock);

			builder->SetInsertPoint(tryLockBlock);
			auto lockedValue = builder->CreateIntToPtr(builder->CreateOr(currentValueAsInt, MakeIntLike(currentValueAsInt, 4)), currentValuePHI->getType());
			auto cmpxchgresult = builder->CreateAtomicCmpXchg(fieldAddress, currentValuePHI, lockedValue, AtomicOrdering::Monotonic, AtomicOrdering::Monotonic);
			auto cmpxchgsuccess = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex + 1, 1));
			outPHI->addIncoming(currentValuePHI, builder->GetInsertBlock());
			builder->CreateCondBr(cmpxchgsuccess, outBlock, loopRecurseBlock);

			builder->SetInsertPoint(loopRecurseBlock);
			auto newCurrentValue = builder->CreateExtractValue(cmpxchgresult, ArrayRef<unsigned int>(extractIndex, 1));
			currentValuePHI->addIncoming(newCurrentValue, builder->GetInsertBlock());
			builder->CreateBr(loopHeadBlock);

			builder->SetInsertPoint(outBlock);
			return outPHI;
		}

		llvm::Value* StructHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
			//return MakeStore(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), value, { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), MakeInt32(fieldindex)) });
		}
		llvm::Value* StructHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return MakeStore(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), value, { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}

		llvm::Value* StructHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
			//return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke&&(NomLambdaOptimizationLevel>0))?2:1)), MakeInt32(fieldindex)) });
		}

		llvm::Value* StructHeader::GetFieldPointer(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}
		llvm::Value* StructHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
			//return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), fieldindex) });
		}
		llvm::Value* StructHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgument(builder, thisObj, MakeInt32(argindex));
			//auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
			//return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), /*builder->CreateSub(builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder, rtstructdesc)),*/ MakeInt32(-(argindex + 1)) }));
		}
		//llvm::Value* StructHeader::GenerateWriteTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, llvm::Value* targ, bool hasRawInvoke)
		//{
		//	auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
		//	return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), /*builder->CreateSub(builder->CreateZExtOrTrunc(builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder, rtstructdesc)), numtype(int32_t)),*/ MakeInt32(- (argindex + 1)) }));
		//}
		llvm::Value* StructHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgsPtr(builder, thisObj);
			//auto rtstructdesc = GenerateReadStructDescriptor(builder, thisObj);
			//return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs)/*, builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder,rtstructdesc))*/ });
			////return MakeLoad(builder, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::TypeArgs), builder->CreateSub(MakeInt32(-((hasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 2 : 1)), builder->CreateAdd(RTStruct::GenerateReadFieldCount(builder, rtstructdesc), RTStruct::GenerateReadTypeArgCount(builder,rtstructdesc))) }));
		}
		//llvm::Value* StructHeader::GenerateWriteCastTypeArgument(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		//{
		//	return MakeStore(builder, targ, builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) }));
		//}
		//llvm::Value* StructHeader::GenerateWriteCastTypeArgumentCMPXCHG(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* argindex, llvm::Value* targ)
		//{
		//	auto argPtr = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), builder->CreateSub(MakeInt32(-1),argindex) });
		//	return builder->CreateAtomicCmpXchg(argPtr, ConstantPointerNull::get(TYPETYPE), targ, AtomicOrdering::AcquireRelease, AtomicOrdering::Acquire);
		//}

		//llvm::Value* StructHeader::GeneratePointerToCastTypeArguments(NomBuilder& builder, llvm::Value* thisObj)
		//{
		//	return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::CastTypeArgs), MakeInt32(0) });
		//}

		void StructHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Value* descriptorRef, const std::vector<NomInstruction*>* instructions)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetStructAlloc(fun->getParent());
			llvm::Value* extraSlots = RTStruct::GenerateReadPreallocatedSlots(builder, descriptorRef);

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(env->Struct->Fields.size()), MakeInt<size_t>(typeArguments.size()), extraSlots }), GetLLVMType()->getPointerTo());

			(*env)[0] = NomValue(newmem, &NomDynamicType::Instance());

			for (auto instr : *instructions)
			{
				instr->Compile(builder, env, 0);
			}

			for (auto field : env->Struct->Fields)
			{
				GenerateWriteField(builder, newmem, field->Index, EnsurePacked(builder, (*env)[field->ValueRegister]), NomLambdaOptimizationLevel > 0 && env->Struct->GetHasRawInvoke());
			}

			//int argIndex = 0;
			//for (auto targ : typeArguments)
			//{
			//	GenerateWriteTypeArgument(builder, newmem, argIndex, targ, NomLambdaOptimizationLevel > 0 && env->Struct->GetHasRawInvoke());
			//	argIndex++;
			//}
			auto vTablePtr = builder->CreatePointerCast(RTStructuralVTable::Instance().GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo());
			auto sTablePtr = builder->CreateIntToPtr(builder->CreateOr(builder->CreatePtrToInt(descriptorRef, numtype(intptr_t)), MakeInt<intptr_t>((intptr_t)RTDescriptorKind::Struct)), RTStruct::GetLLVMType()->getPointerTo());
			//MakeStore(builder, builder->CreatePointerCast(RTStructuralVTable::Instance().GetLLVMElement(*fun->getParent()), RTVTable::GetLLVMType()->getPointerTo()), newmem, { MakeInt32(StructHeaderFields::RefValueHeader), MakeInt32(RefValueHeaderFields::InterfaceTable) });
			//MakeStore(builder, builder->CreateIntToPtr(builder->CreateOr(builder->CreatePtrToInt(descriptorRef, numtype(intptr_t)), MakeInt<intptr_t>((intptr_t)RTDescriptorKind::Struct)), RTStruct::GetLLVMType()->getPointerTo()), newmem, MakeInt32(StructHeaderFields::StructDescriptor));

			builder->CreateCall(RTConcurrentDictionaryEmplace::Instance().GetLLVMElement(*fun->getParent()), { builder->CreateGEP(newmem,{MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0)}) });

			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, sTablePtr);
			//llvm::Value* instanceKind = MakeInt((unsigned char)RTDescriptorKind::Struct);

			auto refValHeader = builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			//auto argSlotCount = builder->CreateIntToPtr(builder->CreateShl(EnsureIntegerSize(builder, extraSlots, bitsin(intptr_t)), MakeInt<intptr_t>(bitsin(intptr_t) / 2)), POINTERTYPE);

			//RefValueHeader::GenerateInitializerCode(builder, refValHeader, argSlotCount, instanceKind);
			builder->CreateRet(refValHeader);
		}
	}
}