#include "NomField.h"
#include "NomClass.h"
#include "NomConstants.h"
#include "NomClassType.h"
#include "instructions/CastInstruction.h"
#include "NomType.h"
#include "NomString.h"
#include "NomNameRepository.h"
#include <unordered_map>
#include "CompileHelpers.h"
#include "TypeOperations.h"
#include "NomLambda.h"
#include "LambdaHeader.h"
#include "RTOutput.h"
#include "StructHeader.h"
#include "RTStruct.h"
#include "RefValueHeader.h"
#include "RTVTable.h"
#include "RTCast.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CallingConvConf.h"
#include "NomTopType.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		NomTypedField::NomTypedField(NomClass* cls, const ConstantID name, const ConstantID type, Visibility visibility, bool readonly, bool isvolatile) : Name(name), Type(type), Class(cls), visibility(visibility), readonly(readonly), isvolatile(isvolatile)
		{
		}

		NomField::NomField()
		{

		}

		NomDictField::NomDictField(NomStringRef name) : Name(name)
		{

		}

		NomField::~NomField()
		{
		}

		NomTypedField::~NomTypedField()
		{
		}

		NomTypeRef NomTypedField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Class);
			return NomConstants::GetType(&nscmc, Type);
		}

		NomStringRef NomTypedField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}

		NomValue NomTypedField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			auto recinst = Class->GetInstantiation(receiver.GetNomType());
			if (recinst != nullptr)
			{
				llvm::Value* retval = ObjectHeader::ReadField(builder, receiver, this->Index, this->Class->GetHasRawInvoke());
				if (this->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
				{
					retval = builder->CreatePtrToInt(retval, INTTYPE);
				}
				else if (this->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
				{
					retval = builder->CreateBitCast(builder->CreatePtrToInt(retval, INTTYPE), FLOATTYPE);
				}
				NomSubstitutionContextList nscl(recinst->Arguments);
				return NomValue(retval, GetType()->SubstituteSubtyping(&nscl));
			}
			throw new std::exception();
		}

		void NomTypedField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			auto recinst = Class->GetInstantiation(receiver.GetNomType());
			if (recinst != nullptr)
			{
				NomSubstitutionContextList nscl(recinst->Arguments);
				if (!value.GetNomType()->IsSubtype(this->GetType()->SubstituteSubtyping(&nscl)))
				{
					value = CastInstruction::MakeCast(builder, env, value, this->GetType()->SubstituteSubtyping(&nscl));
				}
				if (this->GetType()->IsSubtype(NomIntClass::GetInstance()->GetType(), false))
				{
					value = NomValue(builder->CreateIntToPtr(EnsureUnpackedInt(builder, env, value), REFTYPE), value.GetNomType());
				}
				else if (this->GetType()->IsSubtype(NomFloatClass::GetInstance()->GetType(), false))
				{
					value = NomValue(builder->CreateIntToPtr(builder->CreateBitCast(EnsureUnpackedFloat(builder, env, value), INTTYPE), REFTYPE), value.GetNomType());
				}
				else
				{
					value = EnsurePacked(builder, value);
				}
				ObjectHeader::WriteField(builder, receiver, this->Index, value, this->Class->GetHasRawInvoke());
				return;

			}
			throw new std::exception();
		}

		void NomTypedField::SetIndex(int index) const
		{
			if (Index < 0)
			{
				Index = index;
			}
			else
			{
				throw new std::exception();
			}
		}

		NomDictField* NomDictField::GetInstance(NomStringRef name)
		{
			static std::unordered_map<const NomString*, NomDictField, NomStringHash, NomStringEquality> fields;
			auto ret = fields.find(name);
			if (ret == fields.end())
			{
				fields.emplace(name, NomDictField(name));
				ret = fields.find(name);
			}
			return &(*ret).second;
		}

		NomDictField::~NomDictField()
		{
		}
		NomTypeRef NomDictField::GetType() const
		{
			return NomType::Anything;
		}
		NomStringRef NomDictField::GetName() const
		{
			return Name;
		}
		NomValue NomDictField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			std::string key = GetName()->ToStdString();
			return NomValue(builder->CreatePointerCast(ObjectHeader::CreateDictionaryLoad(builder, env, receiver, MakeInt(NomNameRepository::Instance().GetNameID(key))), REFTYPE, key), NomType::DynamicRef);
		}
		void NomDictField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "DictWriteOut", fun);

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				BasicBlock* realVtableBlock = nullptr, * pureLambdaBlock = nullptr, * pureStructBlock = nullptr, * purePartialAppBlock = nullptr;
				Value* vtableVar;
				RefValueHeader::GenerateVTableTagSwitch(builder, receiver, &vtableVar, &realVtableBlock, &pureLambdaBlock, &pureStructBlock, &purePartialAppBlock);

				if (realVtableBlock != nullptr)
				{
					BasicBlock* classObjectBlock = nullptr, * lambdaBlock = nullptr, * structBlock = nullptr, * partialAppBlock = nullptr, * multiCastBlock = nullptr;
					builder->SetInsertPoint(realVtableBlock);
					RTVTable::GenerateVTableKindSwitch(builder, vtableVar, &classObjectBlock, &lambdaBlock, &structBlock, &partialAppBlock, &multiCastBlock);

					if (classObjectBlock != nullptr)
					{
						builder->SetInsertPoint(classObjectBlock);
						auto fieldStoreFun = RTClass::GenerateReadFieldStore(builder, vtableVar);
						builder->CreateCall(NomClass::GetDynamicFieldStoreType(), fieldStoreFun, { receiver, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), EnsurePacked(builder, value) })->setCallingConv(NOMCC);
						builder->CreateBr(outBlock);
					}
					if (lambdaBlock != nullptr)
					{
						RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Lambdas have no dictionary fields to write to!", lambdaBlock);
					}
					if (structBlock != nullptr)
					{
						builder->SetInsertPoint(structBlock);
						auto structDesc = StructHeader::GenerateReadStructDescriptor(builder, receiver);
						auto fieldStoreFun = RTStruct::GenerateReadFieldStore(builder, structDesc);
						builder->CreateCall(NomStruct::GetDynamicFieldStoreType(), fieldStoreFun, { receiver, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), EnsurePacked(builder, value) })->setCallingConv(NOMCC);
						builder->CreateBr(outBlock);
					}
					if (partialAppBlock != nullptr)
					{
						RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Partial Applications have no dictionary fields to write to!", partialAppBlock);
					}
					if (multiCastBlock != nullptr)
					{
						RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Dictionary field write not implemented for multi-casted values!", multiCastBlock);
					}
				}

				if (pureLambdaBlock != nullptr)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Lambdas have no dictionary fields to write to!", pureLambdaBlock);
				}
				if (pureStructBlock != nullptr)
				{
					builder->SetInsertPoint(pureStructBlock);
					auto structDesc = StructHeader::GenerateReadStructDescriptor(builder, receiver);
					auto fieldStoreFun = RTStruct::GenerateReadFieldStore(builder, structDesc);
					builder->CreateCall(NomStruct::GetDynamicFieldStoreType(), fieldStoreFun, { receiver, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), EnsurePacked(builder, value) })->setCallingConv(NOMCC);
					builder->CreateBr(outBlock);
				}
				if (purePartialAppBlock != nullptr)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Partial Applications have no dictionary fields to write to!", purePartialAppBlock);
				}

			}
			if (packedIntBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Integers have no dictionary fields to write to!", packedIntBlock);
			}
			if (packedFloatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Integers have no dictionary fields to write to!", packedFloatBlock);
			}
			if (primitiveIntBlock != nullptr)
			{
				throw new std::exception();
			}
			if (primitiveFloatBlock != nullptr)
			{
				throw new std::exception();
			}
			if (primitiveBoolBlock != nullptr)
			{
				throw new std::exception();
			}
			builder->SetInsertPoint(outBlock);
		}

		//void NomDictField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		//{
		//	auto recType = receiver->getType();
		//	if (recType->isIntegerTy(1))
		//	{
		//		throw new std::exception();
		//	}
		//	else if (recType->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()))
		//	{
		//		throw new std::exception();
		//	}
		//	else if (recType->isFloatingPointTy())
		//	{
		//		throw new std::exception();
		//	}
		//	else if (!(recType->isPointerTy()))
		//	{
		//		throw new std::exception();
		//	}
		//	BasicBlock* incomingBlock = builder->GetInsertBlock();
		//	Function* fun = incomingBlock->getParent();
		//	auto lookupfun = RTDictionaryLookup::Instance().GetLLVMElement(*env->Module);
		//	auto setfun = RTDictionarySet::Instance().GetLLVMElement(*env->Module);

		//	BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "ERROR", fun);
		//	BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "isRef", fun);
		//	BasicBlock* hasVtableBlock = BasicBlock::Create(LLVMCONTEXT, "hasVtable", fun);
		//	BasicBlock* structBlock = BasicBlock::Create(LLVMCONTEXT, "struct", fun);
		//	BasicBlock* objectBlock = BasicBlock::Create(LLVMCONTEXT, "object", fun);
		//	BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "continue", fun);

		//	auto recTag = builder->CreateTrunc(builder->CreatePtrToInt(receiver, numtype(intptr_t)), inttype(2), "tag");
		//	CreateRefKindSwitch(builder, recTag, errorBlock, refBlock);
		//	//builder->CreateCondBr(builder->CreateICmpEQ(recTag, MakeUInt(2, 0)), refBlock, errorBlock);

		//	builder->SetInsertPoint(refBlock);
		//	auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
		//	auto vtableTag = builder->CreateTrunc(builder->CreatePtrToInt(vtable, numtype(intptr_t)), inttype(3), "vtag");
		//	auto vtableTagSwitch = builder->CreateSwitch(vtableTag, errorBlock, 2);
		//	vtableTagSwitch->addCase(MakeUInt(3, (uint64_t)RTInstanceKind::Object), hasVtableBlock);
		//	vtableTagSwitch->addCase(MakeUInt(3, (uint64_t)RTInstanceKind::Struct), structBlock);

		//	builder->SetInsertPoint(hasVtableBlock);
		//	auto vtableKind = RTVTable::GenerateReadKind(builder, vtable);
		//	auto vtableKindSwitch = builder->CreateSwitch(vtableKind, errorBlock, 2);
		//	vtableKindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Class), objectBlock);
		//	vtableKindSwitch->addCase(MakeInt<RTDescriptorKind>(RTDescriptorKind::Struct), structBlock);

		//	{
		//		builder->SetInsertPoint(structBlock);
		//		auto structDesc = StructHeader::GenerateReadStructDescriptor(builder, receiver);
		//		auto fieldStoreFun = RTStruct::GenerateReadFieldStore(builder, structDesc);
		//		builder->CreateCall(NomStruct::GetDynamicFieldStoreType(), fieldStoreFun, { receiver, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), EnsurePacked(builder, value) })->setCallingConv(NOMCC);

		//		builder->CreateBr(outBlock);
		//		//BasicBlock* isNull = BasicBlock::Create(LLVMCONTEXT, "isNull", fun);
		//		//BasicBlock* notNull = BasicBlock::Create(LLVMCONTEXT, "notNull", fun);
		//		//BasicBlock* notFound = BasicBlock::Create(LLVMCONTEXT, "notFound", fun);
		//		//BasicBlock* foundFieldBlock = BasicBlock::Create(LLVMCONTEXT, "foundField", fun);
		//		//BasicBlock* fieldEntryBlock = BasicBlock::Create(LLVMCONTEXT, "fieldEntry", fun);
		//		//BasicBlock* checkReadonlyFlagBlock = BasicBlock::Create(LLVMCONTEXT, "checkReadonly", fun);
		//		//BasicBlock* tryWriteBlock = BasicBlock::Create(LLVMCONTEXT, "tryWrite", fun);
		//		//BasicBlock* isLockedBlock = BasicBlock::Create(LLVMCONTEXT, "isLocked", fun);

		//		//builder->SetInsertPoint(structBlock);
		//		//auto structDesc = StructHeader::GenerateReadStructDescriptor(builder, receiver);
		//		//auto structDict = RTStruct::GenerateReadDictionary(builder, structDesc);

		//		//auto lookupcall = builder->CreateCall(lookupfun, { structDict, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())) });
		//		//lookupcall->setCallingConv(lookupfun->getCallingConv());
		//		//auto entry = builder->CreatePointerCast(lookupcall, RTDescriptorDictionaryEntry::GetLLVMPointerType(), "dictEntry");
		//		//Value* dictEntryIsNull = GenerateIsNull(builder, entry);
		//		//builder->CreateCondBr(dictEntryIsNull, isNull, notNull);

		//		//builder->SetInsertPoint(isNull);
		//		//auto instanceDict = StructHeader::GenerateReadStructDictionary(builder, receiver);
		//		//auto instancelookupcall = builder->CreateCall(lookupfun, { instanceDict, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())) });
		//		//instancelookupcall->setCallingConv(lookupfun->getCallingConv());
		//		//auto instanceDictEntry = builder->CreatePointerCast(instancelookupcall, REFTYPE->getPointerTo());
		//		//Value* instanceEntryIsNull = GenerateIsNull(builder, instanceDictEntry);
		//		//builder->CreateCondBr(instanceEntryIsNull, foundFieldBlock, notFound);

		//		//builder->SetInsertPoint(notNull);
		//		//auto entryKind = RTDescriptorDictionaryEntry::GenerateReadKind(builder, entry);
		//		//auto entryKindSwitch = builder->CreateSwitch(entryKind, errorBlock, 2);
		//		//entryKindSwitch->addCase(MakeInt<RTDescriptorDictionaryEntryKind>(RTDescriptorDictionaryEntryKind::Field), fieldEntryBlock);
		//		//entryKindSwitch->addCase(MakeInt<RTDescriptorDictionaryEntryKind>(RTDescriptorDictionaryEntryKind::PartialApp), errorBlock); //TODO: implement partial application
		//		////entryKindSwitch->addCase(MakeInt<RTDescriptorDictionaryEntryKind>(RTDescriptorDictionaryEntryKind::Field), dispatcherEntryBlock); -- this isn't a thing anymre

		//		//builder->SetInsertPoint(fieldEntryBlock);
		//		//auto fieldIndex = RTDescriptorDictionaryEntry::GenerateReadIndex(builder, entry);
		//		//auto fieldPointer = StructHeader::GetFieldPointer(builder, receiver, fieldIndex);
		//		//builder->CreateBr(foundFieldBlock);

		//		////builder->SetInsertPoint(partialAppEntryBlock);
		//		////auto partialAppDesc = RTDescriptorDictionaryEntry::GenerateReadTypeOrPartialAppOrDispatcher(builder, entry);
		//		////auto dispatcher = RTPartialApp::GenerateFindDispatcher(builder, partialAppDesc, method->Method->GetDirectTypeArgumentCount(), method->Method->GetArgumentCount());
		//		////auto dispatcherIsNull = GenerateIsNull(builder, dispatcher);
		//		////builder->CreateCondBr(dispatcherIsNull, arityMismatchBlock, haveDispatcherBlock);

		//		////builder->SetInsertPoint(haveDispatcherBlock);
		//		////auto dispatcherCallResult = builder->CreateCall(builder->CreatePointerCast(dispatcher, NomPartialApplication::GetDynamicDispatcherType(method->Method->GetDirectTypeArgumentCount(), method->Method->GetArgumentCount())->getPointerTo()), argsBuf, "dispatcherCallResult");
		//		////dispatcherCallResult->setCallingConv(CallingConv::Fast);
		//		////builder->CreateBr(checkReturnValueBlock);

		//		//builder->SetInsertPoint(foundFieldBlock);
		//		//auto fieldPtrPHI = builder->CreatePHI(fieldPointer->getType(), 2, "fieldPtr");
		//		//fieldPtrPHI->addIncoming(fieldPointer, fieldEntryBlock);
		//		//fieldPtrPHI->addIncoming(instanceDictEntry, isNull);
		//		//auto fieldValue = MakeLoad(builder, fieldPtrPHI, "fieldValue");
		//		//builder->CreateBr(checkReadonlyFlagBlock);

		//		//builder->SetInsertPoint(checkReadonlyFlagBlock);
		//		//auto fieldValuePHI = builder->CreatePHI(REFTYPE, 2, "fieldValuePHI");
		//		//fieldValuePHI->addIncoming(fieldValue, foundFieldBlock);
		//		//auto fieldTag = builder->CreateTrunc(builder->CreatePtrToInt(fieldValuePHI, numtype(intptr_t)), inttype(3), "fieldTag");
		//		//auto fieldTagSwitch = builder->CreateSwitch(fieldTag, errorBlock, 2);
		//		//fieldTagSwitch->addCase(MakeUInt(3, 0), tryWriteBlock);
		//		//fieldTagSwitch->addCase(MakeUInt(3, 4), isLockedBlock);

		//		//builder->SetInsertPoint(tryWriteBlock);
		//		////auto writeProtectedVal = builder->CreateIntToPtr(builder->CreateOr(builder->CreatePtrToInt(fieldValuePHI, inttype(64)), builder->CreateZExt(MakeUInt(3, 4), inttype(64))), REFTYPE, "writeProtectedValue");
		//		//auto cmpx = builder->CreateAtomicCmpXchg(fieldPtrPHI, fieldValuePHI, EnsurePacked(builder, value), AtomicOrdering::Monotonic, AtomicOrdering::Monotonic);
		//		//auto cmpxvalue = builder->CreateExtractValue(cmpx, { 0 });
		//		//auto cmpxsuccess = builder->CreateExtractValue(cmpx, { 1 });
		//		//tryWriteBlock = builder->GetInsertBlock();
		//		//builder->CreateCondBr(cmpxsuccess, outBlock, checkReadonlyFlagBlock);

		//		//fieldValuePHI->addIncoming(cmpxvalue, tryWriteBlock);

		//		//builder->SetInsertPoint(isLockedBlock);
		//		//static const char* isLocked_errorMessage = "Tried to write to read-only field!";
		//		//builder->CreateCall(RTOutput_Fail::GetLLVMElement(*env->Module), GetLLVMPointer(isLocked_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*env->Module)->getCallingConv());
		//		//CreateDummyReturn(builder, fun);

		//		//builder->SetInsertPoint(notFound);
		//		//auto slot = builder->CreateCall(GetAlloc(env->Module), { llvmsizeof(POINTERTYPE) });
		//		//MakeStore(builder, value, builder->CreatePointerCast(slot, REFTYPE->getPointerTo()));
		//		//builder->CreateCall(setfun, { instanceDict, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())) , slot });
		//		//builder->CreateBr(outBlock);
		//	}

		//	{
		//		//BasicBlock* isNull = BasicBlock::Create(LLVMCONTEXT, "isNull", fun);
		//		//BasicBlock* notNull = BasicBlock::Create(LLVMCONTEXT, "notNull", fun);
		//		//BasicBlock* fieldEntryBlock = BasicBlock::Create(LLVMCONTEXT, "fieldEntry", fun);
		//		////BasicBlock* checkReadonlyFlagBlock = BasicBlock::Create(LLVMCONTEXT, "checkReadonly", fun);
		//		//BasicBlock* canWrite = BasicBlock::Create(LLVMCONTEXT, "canWrite", fun);

		//		builder->SetInsertPoint(objectBlock);
		//		auto fieldStoreFun = RTClass::GenerateReadFieldStore(builder, vtable);
		//		builder->CreateCall(NomClass::GetDynamicFieldStoreType(), fieldStoreFun, { receiver, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())), EnsurePacked(builder, value) })->setCallingConv(NOMCC);

		//		builder->CreateBr(outBlock);

		//		//auto classDesc = ObjectHeader::GenerateReadVTablePointer(builder, receiver);
		//		//auto classDict = RTClass::GenerateReadDescriptorDictionary(builder, classDesc);

		//		//auto lookupcall = builder->CreateCall(lookupfun, { classDict, MakeInt<DICTKEYTYPE>(NomNameRepository::Instance().GetNameID(this->Name->ToStdString())) });
		//		//lookupcall->setCallingConv(lookupfun->getCallingConv());
		//		//auto entry = builder->CreatePointerCast(lookupcall, RTDescriptorDictionaryEntry::GetLLVMPointerType(), "dictEntry");
		//		//Value* dictEntryIsNull = GenerateIsNull(builder, entry);
		//		//builder->CreateCondBr(dictEntryIsNull, isNull, notNull);

		//		//builder->SetInsertPoint(notNull);
		//		//auto entryKind = RTDescriptorDictionaryEntry::GenerateReadKind(builder, entry);
		//		//auto entryKindSwitch = builder->CreateSwitch(entryKind, errorBlock, 2);
		//		//entryKindSwitch->addCase(MakeInt<RTDescriptorDictionaryEntryKind>(RTDescriptorDictionaryEntryKind::Field), fieldEntryBlock);
		//		//entryKindSwitch->addCase(MakeInt<RTDescriptorDictionaryEntryKind>(RTDescriptorDictionaryEntryKind::PartialApp), errorBlock);


		//		//builder->SetInsertPoint(isNull);
		//		//static const char* isNull_errorMessage = "Field not found!";
		//		//builder->CreateCall(RTOutput_Fail::GetLLVMElement(*env->Module), GetLLVMPointer(isNull_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*env->Module)->getCallingConv());
		//		//CreateDummyReturn(builder, fun);

		//		//builder->SetInsertPoint(fieldEntryBlock);
		//		//auto fieldIndex = RTDescriptorDictionaryEntry::GenerateReadIndex(builder, entry);
		//		//auto fieldType = builder->CreatePointerCast(RTDescriptorDictionaryEntry::GenerateReadTypeOrPartialAppOrDispatcher(builder, entry), TYPETYPE);
		//		//auto isReadOnly = RTDescriptorDictionaryEntry::GenerateReadFlags(builder, entry);
		//		//auto castResult = RTCast::GenerateMonotonicCast(builder, env, value, fieldType);
		//		//builder->CreateCondBr(builder->CreateAnd(castResult, builder->CreateICmpEQ(isReadOnly, ConstantInt::get(isReadOnly->getType(), 0))), canWrite, errorBlock);

		//		//builder->SetInsertPoint(canWrite);
		//		//ObjectHeader::WriteField(builder, receiver, fieldIndex, value);
		//		//builder->CreateBr(outBlock);
		//	}

		//	builder->SetInsertPoint(errorBlock);
		//	static const char* generic_errorMessage = "ERROR in dictionary write!";
		//	builder->CreateCall(RTOutput_Fail::GetLLVMElement(*env->Module), GetLLVMPointer(generic_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*env->Module)->getCallingConv());
		//	CreateDummyReturn(builder, fun);

		//	builder->SetInsertPoint(outBlock);
		//}
		NomClosureField::NomClosureField(NomLambda* lambda, const ConstantID name, const ConstantID type, const int index) : Name(name), Type(type), Lambda(lambda), Index(index)
		{
		}
		NomClosureField::~NomClosureField()
		{
		}
		NomTypeRef NomClosureField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Lambda);
			return NomConstants::GetType(&nscmc, Type);
		}
		NomStringRef NomClosureField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}
		NomValue NomClosureField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			return NomValue(LambdaHeader::GenerateReadField(builder, Lambda, receiver, Index), GetType());
		}
		void NomClosureField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			throw new std::exception();
		}
		NomStructField::NomStructField(NomStruct* structure, const ConstantID name, const ConstantID type, bool isReadOnly, const int index, RegIndex valueRegister) : readonly(isReadOnly), Name(name), Type(type), Structure(structure), Index(index), ValueRegister(valueRegister)
		{
		}
		NomStructField::~NomStructField()
		{
		}
		NomTypeRef NomStructField::GetType() const
		{
			NomSubstitutionContextMemberContext nscmc(Structure);
			return NomConstants::GetType(&nscmc, Type);
		}
		NomStringRef NomStructField::GetName() const
		{
			return NomConstants::GetString(Name)->GetText();
		}
		NomValue NomStructField::GenerateRead(NomBuilder& builder, CompileEnv* env, NomValue receiver) const
		{
			llvm::Value* retval = StructHeader::GenerateReadField(builder, receiver, Index, this->Structure->GetHasRawInvoke()); //loadinst;
			return NomValue(retval, GetType());
		}
		void NomStructField::GenerateWrite(NomBuilder& builder, CompileEnv* env, NomValue receiver, NomValue value) const
		{
			if (!value.GetNomType()->IsSubtype(this->GetType()))
			{
				value = CastInstruction::MakeCast(builder, env, value, this->GetType());
			}
			value = EnsurePacked(builder, value);
			StructHeader::GenerateWriteField(builder, receiver, Index, value, Structure->GetHasRawInvoke());
			return;
		}
	}
}