#include "RTVTable.h"
#include "RTInterfaceTableEntry.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "RTDescriptor.h"
#include "IMT.h"
#include "ObjectHeader.h"
#include "RecordHeader.h"
#include "LambdaHeader.h"
#include "RTInstanceType.h"
#include "RTOutput.h"
#include "RTConfig.h"
#include "RTInterface.h"
#include "NomNameRepository.h"
#include "RefValueHeader.h"
#include "Metadata.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTVTable::GetLLVMType()
		{
			static StructType* rtitt = StructType::create(LLVMCONTEXT, "RT_NOM_InterfaceVTable");
			static bool once = true;
			if (once)
			{
				once = false;
				rtitt->setBody(
					arrtype(POINTERTYPE,0),													//Class method table (grows upward)
					numtype(RTDescriptorKind),												//Kind
					arrtype(GetIMTFunctionType()->getPointerTo(), IMTsize),					//Interface method table
					arrtype(GetDynamicDispatchListEntryType()->getPointerTo(), IMTsize),	//Dynamic dispatcher retrieval
					GetFieldReadFunctionType()->getPointerTo(),								//field lookup
					GetFieldWriteFunctionType()->getPointerTo()								//field store
				);
			}
			return rtitt;
		}
		llvm::Constant* RTVTable::CreateConstant(RTDescriptorKind kind, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable, llvm::Constant* fieldLookupFunction, llvm::Constant* fieldStoreFunction)
		{
			return ConstantStruct::get(GetLLVMType(), ConstantArray::get(arrtype(POINTERTYPE, 0), {}), MakeInt<RTDescriptorKind>(kind), interfaceMethodTable, dynamicDispatcherTable, fieldLookupFunction, fieldStoreFunction);
		}
		llvm::Value* RTVTable::GenerateReadKind(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::Kind), "VTableKind", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadInterfaceMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Constant* index)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::InterfaceMethodTable), index }), "IMTEntry", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset)
		{
			return MakeInvariantLoad(builder, builder->CreateGEP(builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTVTableFields::MethodTable), builder->CreateSub(MakeInt32(-1), offset) }), "MethodPointer", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadReadFieldFunction(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::ReadField), "readFieldFun", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateReadWriteFieldFunction(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			return MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), MakeInt32(RTVTableFields::WriteField), "writeFieldFun", AtomicOrdering::NotAtomic);
		}
		llvm::Value* RTVTable::GenerateFindDynamicDispatcherPair(NomBuilder& builder, Value* refValue, Value* vtablePtr, size_t name)
		{
			return GenerateFindDynamicDispatcherPair(builder, refValue, vtablePtr, MakeUInt(64, name), MakeUInt(32, name % IMTsize));
		}
		llvm::Value* RTVTable::GenerateHasRawInvoke(NomBuilder& builder, Value* vtablePtr)
		{
			return builder->CreateTrunc(builder->CreateLShr(builder->CreatePtrToInt(vtablePtr, numtype(intptr_t)), MakeInt<intptr_t>(3)), inttype(1));
		}
		llvm::Value* RTVTable::GenerateIsNominalValue(NomBuilder& builder, Value* vtablePtr)
		{
			return builder->CreateTrunc(builder->CreateLShr(builder->CreatePtrToInt(vtablePtr, numtype(intptr_t)), MakeInt<intptr_t>(4)), inttype(1));
		}

		void RTVTable::GenerateFreezeMethodField(NomBuilder& builder, Value* refValue, Value* vtablePtr, Value* name, Value* tableIndex)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			auto fun = origBlock->getParent();

			BasicBlock* structuralBlock = BasicBlock::Create(LLVMCONTEXT, "freeze$start", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "freeze$out", fun);

			auto isNominalValue = GenerateIsNominalValue(builder, vtablePtr);
			builder->CreateCondBr(isNominalValue, outBlock, structuralBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(structuralBlock);
			{
				auto listPtr = MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(RTVTableFields::DynamicDispatcherTable), tableIndex }, "DDTE", AtomicOrdering::NotAtomic);

				BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Head", fun);
				BasicBlock* loopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Body", fun);
				BasicBlock* loopMatchBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Match", fun);
				BasicBlock* loopMatchFieldBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Match$Field", fun);
				BasicBlock* loopDictionaryBlock = RTOutput_Fail::GenerateFailUnimplementedBlock(builder);

				builder->CreateBr(loopHeadBlock);

				builder->SetInsertPoint(loopHeadBlock);
				auto indexPHI = builder->CreatePHI(inttype(32), 2);
				indexPHI->addIncoming(MakeInt32(0), structuralBlock);
				auto currentEntry = builder->CreateGEP(listPtr, indexPHI, "currentEntry");
				auto currentKey = MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Key), "DDTEKey", AtomicOrdering::NotAtomic);
				auto currentKeyIsNotNull = builder->CreateICmpNE(currentKey, MakeIntLike(currentKey, 0), "keyIsNotNull");
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { currentKeyIsNotNull, MakeUInt(1,1) });
				builder->CreateCondBr(currentKeyIsNotNull, loopBodyBlock, loopDictionaryBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(loopBodyBlock);
				auto isMatch = builder->CreateICmpEQ(currentKey, builder->CreateZExtOrTrunc(name, currentKey->getType()), "keyMatch");
				auto nextIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1));
				indexPHI->addIncoming(nextIndex, builder->GetInsertBlock());
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isMatch, MakeUInt(1,1) });
				builder->CreateCondBr(isMatch, loopMatchBlock, loopHeadBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(loopMatchBlock);
				auto flag = MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Flags), "DDTEFlags", AtomicOrdering::NotAtomic);
				auto isMethod = builder->CreateICmpEQ(flag, MakeIntLike(flag, 0), "isMethod");
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isMethod, MakeUInt(1,1) });
				builder->CreateCondBr(isMethod, outBlock, loopMatchFieldBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(loopMatchFieldBlock);
				auto fieldIndex = builder->CreatePtrToInt(MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Dispatcher), "fieldNumber", AtomicOrdering::Unordered), numtype(intptr_t));
				auto fieldValue = RecordHeader::GenerateReadAndLockField(builder, refValue, fieldIndex, false); //hasRawInvoke is false because this table is already filled with appropriately changed indices
				//fieldValue is definitely a reference value because locking only works on reference values
				static const char* noLambdaMsg = "Tried to load invokable value from field, but value has no lambda method!";
				BasicBlock* noLambdaBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, noLambdaMsg);
				auto hasLambda = GenerateHasRawInvoke(builder, RefValueHeader::GenerateReadVTablePointer(builder, fieldValue));
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { hasLambda, MakeUInt(1,1) });
				builder->CreateCondBr(hasLambda, outBlock, noLambdaBlock, GetLikelyFirstBranchMetadata());
			}

			builder->SetInsertPoint(outBlock);
		}
		llvm::Value* RTVTable::GenerateFindDynamicDispatcherPair(NomBuilder& builder, Value* refValue, Value* vtablePtr, Value* name, Value* tableIndex)
		{
			auto listPtr = MakeInvariantLoad(builder, builder->CreatePointerCast(vtablePtr, GetLLVMType()->getPointerTo()), { MakeInt32(RTVTableFields::DynamicDispatcherTable), tableIndex }, "DDTE", AtomicOrdering::NotAtomic);
			
			BasicBlock* origBlock = builder->GetInsertBlock();
			auto fun = origBlock->getParent();

			BasicBlock* loopHeadBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Head", fun);
			BasicBlock* loopBodyBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Body", fun);
			BasicBlock* loopMatchBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Match", fun);
			BasicBlock* loopMatchFieldBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Match$Field", fun);
			BasicBlock* loopMatchMethodBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookupLoop$Match$Method", fun);
			BasicBlock* loopDictionaryBlock = RTOutput_Fail::GenerateFailUnimplementedBlock(builder);
			BasicBlock* checkLambdaExistsBlock = BasicBlock::Create(LLVMCONTEXT, "checkLambdaExists", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "DDLookup$out", fun);

			builder->SetInsertPoint(outBlock);
			auto outPHI = builder->CreatePHI(GetDynamicDispatcherLookupResultType(), 3);

			builder->SetInsertPoint(origBlock);

			builder->CreateBr(loopHeadBlock);

			builder->SetInsertPoint(loopHeadBlock);
			auto indexPHI = builder->CreatePHI(inttype(32), 2);
			indexPHI->addIncoming(MakeInt32(0), origBlock);
			auto currentEntry = builder->CreateGEP(listPtr, indexPHI, "currentEntry");
			auto currentKey = MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Key), "DDTEKey", AtomicOrdering::NotAtomic);
			auto currentKeyIsNotNull = builder->CreateICmpNE(currentKey, MakeIntLike(currentKey,0), "keyIsNotNull");
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { currentKeyIsNotNull, MakeUInt(1,1) });
			builder->CreateCondBr(currentKeyIsNotNull, loopBodyBlock, loopDictionaryBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(loopBodyBlock);
			auto isMatch = builder->CreateICmpEQ(currentKey, builder->CreateZExtOrTrunc(name, currentKey->getType()), "keyMatch");
			auto nextIndex = builder->CreateAdd(indexPHI, MakeIntLike(indexPHI, 1));
			indexPHI->addIncoming(nextIndex, builder->GetInsertBlock());
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isMatch, MakeUInt(1,1) });
			builder->CreateCondBr(isMatch, loopMatchBlock, loopHeadBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(loopMatchBlock);
			auto flag = MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Flags), "DDTEFlags", AtomicOrdering::NotAtomic);
			auto isMethod = builder->CreateICmpEQ(flag, MakeIntLike(flag, 0), "isMethod");
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isMethod, MakeUInt(1,1) });
			builder->CreateCondBr(isMethod, loopMatchMethodBlock, loopMatchFieldBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(loopMatchMethodBlock);
			{
				auto dispatcher = MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Dispatcher), "dispatcher", AtomicOrdering::Unordered);
				auto pairfirst = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), dispatcher, { 0 });
				auto retval = builder->CreateInsertValue(pairfirst, EnsurePackedUnpacked(builder, refValue, POINTERTYPE), { 1 });
				outPHI->addIncoming(retval, builder->GetInsertBlock());
				builder->CreateBr(outBlock);
			}

			builder->SetInsertPoint(checkLambdaExistsBlock);
			BasicBlock* loadLambdaBlock = BasicBlock::Create(LLVMCONTEXT, "loadLambda", fun);
			static const char* noLambdaMsg = "Tried to load invokable value from field, but value has no lambda method!";
			BasicBlock* noLambdaBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, noLambdaMsg);
			auto fieldValuePHI = builder->CreatePHI(REFTYPE, 2, "fieldValue");
			auto hasLambda = GenerateHasRawInvoke(builder, RefValueHeader::GenerateReadVTablePointer(builder, fieldValuePHI));
			builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { hasLambda, MakeUInt(1,1) });
			builder->CreateCondBr(hasLambda, loadLambdaBlock, noLambdaBlock, GetLikelyFirstBranchMetadata());

			builder->SetInsertPoint(loadLambdaBlock);
			{
				auto lambda = RefValueHeader::GenerateReadRawInvoke(builder, fieldValuePHI);
				auto pairfirst = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), builder->CreatePointerCast(lambda, GetIMTFunctionType()->getPointerTo()), { 0 });
				auto retval = builder->CreateInsertValue(pairfirst, EnsurePackedUnpacked(builder, fieldValuePHI, POINTERTYPE), { 1 });
				outPHI->addIncoming(retval, builder->GetInsertBlock());
				builder->CreateBr(outBlock);
			}

			builder->SetInsertPoint(loopMatchFieldBlock);
			{
				BasicBlock* classFieldBlock = BasicBlock::Create(LLVMCONTEXT, "classFieldRead", fun);
				BasicBlock* recordFieldBlock = BasicBlock::Create(LLVMCONTEXT, "recordFieldRead", fun);
				auto fieldIndex = builder->CreatePtrToInt(MakeInvariantLoad(builder, currentEntry, MakeInt32(DynamicDispatchListEntryFields::Dispatcher), "fieldNumber", AtomicOrdering::Unordered), numtype(intptr_t));
				auto isNominalValue = GenerateIsNominalValue(builder, vtablePtr);
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { isNominalValue, MakeUInt(1,1) });
				builder->CreateCondBr(isNominalValue, classFieldBlock, recordFieldBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(classFieldBlock);
				{
					//BasicBlock* loadClassFieldBlock = BasicBlock::Create(LLVMCONTEXT, "classFieldRead$load", fun);
					auto actualIndex = builder->CreateTrunc(builder->CreateLShr(fieldIndex, MakeIntLike(fieldIndex, 32)), inttype(32));
					//auto indexFlags = builder->CreateTrunc(fieldIndex, inttype(2));

					//auto noSpecialFlags = builder->CreateICmpEQ(indexFlags, MakeIntLike(indexFlags, 0));
					//builder->CreateCondBr(noSpecialFlags, loadClassFieldBlock, noLambdaBlock);

					//builder->SetInsertPoint(loadClassFieldBlock);
					auto fieldValue = ObjectHeader::ReadField(builder, refValue, actualIndex, false); //hasRawInvoke is false because this table is already filled with appropriately changed indices

					BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, fieldValue, &refValueBlock, &intBlock, &floatBlock, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

					if (refValueBlock != nullptr)
					{
						builder->SetInsertPoint(refValueBlock);
						fieldValuePHI->addIncoming(fieldValue, builder->GetInsertBlock());
						builder->CreateBr(checkLambdaExistsBlock);
					}
					if (intBlock != nullptr)
					{
						builder->SetInsertPoint(intBlock);
						builder->CreateBr(noLambdaBlock);
					}
					if (floatBlock != nullptr)
					{
						builder->SetInsertPoint(floatBlock);
						builder->CreateBr(noLambdaBlock);
					}
				}

				builder->SetInsertPoint(recordFieldBlock);
				{
					auto fieldValue = RecordHeader::GenerateReadField(builder, refValue, fieldIndex, false); //hasRawInvoke is false because this table is already filled with appropriately changed indices
					fieldValuePHI->addIncoming(fieldValue, builder->GetInsertBlock());
					builder->CreateBr(checkLambdaExistsBlock);
				}
			}

			builder->SetInsertPoint(outBlock);
			return outPHI;
		}
	}
}