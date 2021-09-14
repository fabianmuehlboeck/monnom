#include "RecordHeader.h"
#include "RefValueHeader.h"
#include "CompileHelpers.h"
#include "NomVMInterface.h"
#include "NomRecord.h"
#include "NomField.h"
#include "NomDynamicType.h"
#include "RTCompileConfig.h"
#include "RTVTable.h"
#include "RTOutput.h"
#include "Metadata.h"
#include "StructuralValueHeader.h"
#include "RTDictionary.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RecordHeader::GetLLVMType()
		{
			static llvm::StructType* shst = StructType::create(LLVMCONTEXT, "RT_NOM_StructHeader");
			static bool once = true;
			if (once)
			{
				once = false;
				shst->setBody(
					StructuralValueHeader::GetLLVMType(),													//common structural value stuff
					//arrtype(llvm::Type::getInt8Ty(LLVMCONTEXT), GetConcurrentDictionarySize()),				//instance dictionary
					POINTERTYPE,																			//instance dictionary
					arrtype(REFTYPE, 0)																		//Fields
				);
			}
			return shst;
		}

		llvm::Value* RecordHeader::GenerateReadStructDictionary(NomBuilder& builder, llvm::Value* objPointer)
		{
			return builder->CreateGEP(builder->CreatePointerCast(objPointer, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0) });
			//return MakeLoad(builder, objPointer, GetLLVMType()->getPointerTo(), MakeInt32(StructHeaderFields::InstanceDictionary), "structInstanceDictionary");
		}

		/// <summary>
		/// Locks the value of a given (reference) value by making its LSB tag 0x4. This is only a valid operation if the field is already locked this way,
		/// or if the LSB tag is 0x0 (i.e. primitives cannot be locked). Returns the (untagged) value of the now locked field.
		/// </summary>
		/// <param name="builder"></param>
		/// <param name="thisObj"></param>
		/// <param name="fieldindex"></param>
		/// <param name="hasRawInvoke"></param>
		/// <returns></returns>
		llvm::Value* RecordHeader::GenerateReadAndLockField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			static unsigned int extractIndex[2] = { 0,1 };
			static const char* lockErrorMsg = "Cannot lock primitive value!";
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "fieldLocked", builder->GetInsertBlock()->getParent());
			BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "lockLoop$head", builder->GetInsertBlock()->getParent());
			BasicBlock* loopRecurseBlock = BasicBlock::Create(LLVMCONTEXT, "lockLoop$recurse", builder->GetInsertBlock()->getParent());
			BasicBlock* tryLockBlock = BasicBlock::Create(LLVMCONTEXT, "tryLockField", builder->GetInsertBlock()->getParent());
			BasicBlock* checkPrimtiveBlock = BasicBlock::Create(LLVMCONTEXT, "checkPrimitive", builder->GetInsertBlock()->getParent());
			BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, lockErrorMsg);
			auto fieldAddress = builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldindex });
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
			outPHI->addIncoming(currentValuePHI, checkPrimtiveBlock);

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

		llvm::Value* RecordHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
		}
		llvm::Value* RecordHeader::GenerateWriteField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, llvm::Value* value, bool hasRawInvoke)
		{
			return MakeStore(builder, value, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
		}

		llvm::Value* RecordHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, int32_t fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), MakeInt32(fieldindex) });
		}

		llvm::Value* RecordHeader::GetFieldPointer(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return builder->CreateGEP(builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(StructHeaderFields::Fields), fieldindex });
		}
		llvm::Value* RecordHeader::GenerateReadField(NomBuilder& builder, llvm::Value* thisObj, llvm::Value* fieldindex, bool hasRawInvoke)
		{
			return MakeLoad(builder, builder->CreatePointerCast(thisObj, GetLLVMType()->getPointerTo()), { MakeInt32(StructHeaderFields::Fields), fieldindex });
		}
		llvm::Value* RecordHeader::GenerateReadTypeArgument(NomBuilder& builder, llvm::Value* thisObj, int32_t argindex, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgument(builder, thisObj, MakeInt32(argindex));
		}
		llvm::Value* RecordHeader::GeneratePointerToTypeArguments(NomBuilder& builder, llvm::Value* thisObj, bool hasRawInvoke)
		{
			return StructuralValueHeader::GenerateReadTypeArgsPtr(builder, thisObj);
		}


		void RecordHeader::GenerateConstructorCode(NomBuilder& builder, llvm::ArrayRef<llvm::Value*> typeArguments, StructInstantiationCompileEnv* env, llvm::Constant* descriptorRef, const std::vector<NomInstruction*>* instructions, llvm::Constant* rawInvokePtr)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			Function* allocfun = GetRecordAlloc(fun->getParent());

			llvm::Value* newmem = builder->CreatePointerCast(builder->CreateCall(allocfun, { MakeInt<size_t>(env->Record->Fields.size()), MakeInt<size_t>(typeArguments.size())}), GetLLVMType()->getPointerTo());

			(*env)[0] = NomValue(newmem, &NomDynamicType::Instance());

			for (auto instr : *instructions)
			{
				instr->Compile(builder, env, 0);
			}

			for (auto field : env->Record->Fields)
			{
				GenerateWriteField(builder, newmem, field->Index, EnsurePacked(builder, (*env)[field->ValueRegister]), NomLambdaOptimizationLevel > 0 && env->Record->GetHasRawInvoke());
			}

			auto vTablePtr = ConstantExpr::getPointerCast(descriptorRef, RTVTable::GetLLVMType()->getPointerTo());

			MakeInvariantStore(builder, ConstantPointerNull::get(POINTERTYPE), newmem, MakeInt32(StructHeaderFields::InstanceDictionary), AtomicOrdering::NotAtomic);

			//builder->CreateCall(RTConcurrentDictionaryEmplace::Instance().GetLLVMElement(*fun->getParent()), { builder->CreateGEP(newmem,{MakeInt32(0), MakeInt32(StructHeaderFields::InstanceDictionary), MakeInt32(0)}) });

			StructuralValueHeader::GenerateInitializationCode(builder, newmem, typeArguments, vTablePtr, rawInvokePtr);

			auto refValHeader = builder->CreateGEP(newmem, { MakeInt32(0), MakeInt32(StructHeaderFields::StructValueHeader), MakeInt32(StructuralValueHeaderFields::RefValueHeader) }, "RefValHeader");

			builder->CreateRet(refValHeader);
		}
	}
}