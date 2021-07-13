#include "RTPartialApp.h"
#include "Defs.h"
#include "llvm/IR/DerivedTypes.h"
#include "RTDictionary.h"
#include "CompileHelpers.h"
#include "ObjectHeader.h"
#include "NomNameRepository.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomPartialApplication.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RTPartialApp::GetLLVMType()
		{
			static llvm::StructType* pat = llvm::StructType::create(LLVMCONTEXT, { numtype(uint64_t), arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), 0)  }, "RT_NOM_PartialAppDescriptor");
			return pat;
		}
		llvm::Constant* GetKeyConstant(uint32_t typeargcount, uint32_t argcount)
		{
			return MakeInt<uint64_t>((((uint64_t)typeargcount) << 32) + (uint64_t)argcount);
		}
		llvm::Constant* RTPartialApp::CreateConstant(llvm::ArrayRef<std::pair<std::pair<uint32_t, uint32_t>, llvm::Constant*>> entries)
		{
			llvm::Constant ** cbuf = makealloca(llvm::Constant*, entries.size());
			int pos = 0;
			for (auto &entry : entries)
			{
				cbuf[pos] = llvm::ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), GetKeyConstant(std::get<0>(std::get<0>(entry)), std::get<1>(std::get<0>(entry))), std::get<1>(entry));
			}
			auto arr = llvm::ConstantArray::get(arrtype(llvm::StructType::get(numtype(uint64_t), POINTERTYPE), entries.size()), llvm::ArrayRef<llvm::Constant*>(cbuf, entries.size()));
			return ConstantStruct::get(llvm::StructType::get(numtype(uint64_t), arr->getType()), MakeInt<uint64_t>(entries.size()), arr);
		}
		//llvm::Constant* RTPartialApp::CreateConstant(const void* nomlink, const void* dictionary, RTDescriptorKind kind)
		//{
		//	return ConstantStruct::get(GetLLVMType(), MakeInt<RTDescriptorKind>(kind), GetLLVMPointer(nomlink), ConstantExpr::getPointerCast(GetLLVMPointer(dictionary), DICTIONARYTYPE));
		//}
		//llvm::Value* RTPartialApp::GenerateReadDescriptorKind(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr)
		//{
		//	return MakeLoad(builder, mod, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTPartialAppFields::Kind) }));
		//}
		//llvm::Value* RTPartialApp::GenerateReadDescriptorLink(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr)
		//{
		//	return MakeLoad(builder, mod, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTPartialAppFields::NomLink) }));;
		//}
		//llvm::Value* RTPartialApp::GenerateReadDescriptorDictionary(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr)
		//{
		//	return MakeLoad(builder, mod, builder->CreateGEP(builder->CreatePointerCast(descriptorPtr, GetLLVMType()->getPointerTo()), { MakeInt32(0), MakeInt32(RTPartialAppFields::Dictionary) }));;
		//}

		//llvm::Value* RTPartialApp::GenerateCheckIsExpando(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr)
		//{
		//	return MakeInt(1, (uint64_t)0);
		//}
		/*llvm::Constant* GetKeyConstant(llvm::Constant* typeargcount, llvm::Constant* argcount)
		{
			return ConstantExpr::getAdd(ConstantExpr::getShl(ConstantExpr::getZExt(typeargcount, numtype(uint64_t)), MakeInt<uint64_t>(32)), ConstantExpr::getZExt(argcount, numtype(uint64_t)));
		}*/

		llvm::Value* RTPartialApp::GenerateFindDispatcher(NomBuilder& builder, llvm::Value* partialAppDesc, uint32_t typeargcount, uint32_t argcount)
		{
			BasicBlock* incoming = builder->GetInsertBlock();
			BasicBlock* loopHead = BasicBlock::Create(LLVMCONTEXT, "palookup_head", incoming->getParent());
			BasicBlock* loopBody = BasicBlock::Create(LLVMCONTEXT, "palookup_body", incoming->getParent());
			BasicBlock* foundBlock = BasicBlock::Create(LLVMCONTEXT, "palookup_found", incoming->getParent());
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "palookup_out", incoming->getParent());

			auto padesc = builder->CreatePointerCast(partialAppDesc, GetLLVMType()->getPointerTo());
			auto entryCount = builder->CreateTrunc(MakeLoad(builder, padesc, MakeInt32(RTPartialAppFields::NumEntries), "pa_entrycount"), numtype(uint32_t));
			builder->CreateBr(loopHead);

			builder->SetInsertPoint(loopHead);
			auto countPHI = builder->CreatePHI(numtype(uint32_t), 2);
			countPHI->addIncoming(entryCount, incoming);
			auto countGTzero = builder->CreateICmpUGT(countPHI, MakeInt<uint32_t>(0));
			builder->CreateCondBr(countGTzero, loopBody, outBlock);

			builder->SetInsertPoint(loopBody);
			auto countminusone = builder->CreateSub(countPHI, MakeInt<uint32_t>(1));
			auto entryKey = MakeLoad(builder, padesc, { MakeInt32(RTPartialAppFields::Entries), countminusone, MakeInt32(RTPartialAppEntryFields::Key) });
			auto entryKeyMatch = builder->CreateICmpEQ(entryKey, GetKeyConstant(typeargcount, argcount));
			countPHI->addIncoming(countminusone, loopBody);
			builder->CreateCondBr(entryKeyMatch, foundBlock, loopHead);

			builder->SetInsertPoint(foundBlock);
			auto dispatcher = MakeLoad(builder, padesc, { MakeInt32(RTPartialAppFields::Entries), countminusone, MakeInt32(RTPartialAppEntryFields::Dispatcher) });
			auto dispatcherCast = builder->CreatePointerCast(dispatcher, NomPartialApplication::GetDynamicDispatcherType(typeargcount, argcount)->getPointerTo());
			builder->CreateBr(outBlock);

			builder->SetInsertPoint(outBlock);
			auto outPHI = builder->CreatePHI(NomPartialApplication::GetDynamicDispatcherType(typeargcount, argcount)->getPointerTo(), 2);
			outPHI->addIncoming(dispatcherCast, foundBlock);
			outPHI->addIncoming(ConstantPointerNull::get(NomPartialApplication::GetDynamicDispatcherType(typeargcount, argcount)->getPointerTo()), loopHead);
			return outPHI;
		}

		//llvm::Value* RTPartialApp::GenerateFindDispatcher(NomBuilder& builder, llvm::Module& mod, llvm::Value* descriptorPtr, llvm::ConstantInt* key)
		//{
		//	BasicBlock* nullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* notnullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* dispatcherBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());

		//	auto dictptr = GenerateReadDescriptorDictionary(builder, mod, descriptorPtr);
		//	auto lookupfun = RTDictionaryLookup::Instance().GetLLVMElement(mod);
		//	auto lookupcall = builder->CreateCall(lookupfun, { dictptr, key });
		//	lookupcall->setCallingConv(lookupfun->getCallingConv());
		//	auto entry = builder->CreatePointerCast(lookupcall, GetDescriptorDictionaryEntryType()->getPointerTo());
		//	auto entryIsNull = GenerateIsNull(builder, entry);
		//	builder->CreateCondBr(entryIsNull, nullBlock, notnullBlock);

		//	builder->SetInsertPoint(nullBlock);
		//	auto nullval = ConstantPointerNull::get(POINTERTYPE);
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(notnullBlock);
		//	auto nullval2 = ConstantPointerNull::get(POINTERTYPE);
		//	auto kind = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::Kind) }));
		//	auto kindSwitch = builder->CreateSwitch(kind, mergeBlock, 1);
		//	kindSwitch->addCase(MakeInt<DescriptorDictionaryEntryKind>(DescriptorDictionaryEntryKind::Dispatcher), dispatcherBlock);

		//	builder->SetInsertPoint(dispatcherBlock);
		//	auto dispatcher = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::TypeOrPartialAppOrDispatcher) }));
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(mergeBlock);
		//	auto phi = builder->CreatePHI(POINTERTYPE, 3);
		//	phi->addIncoming(nullval, nullBlock);
		//	phi->addIncoming(nullval2, notnullBlock);
		//	phi->addIncoming(dispatcher, dispatcherBlock);
		//	return phi;
		//	//BasicBlock* nullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	//BasicBlock* notnullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());

		//	//auto dictptr = GenerateReadDescriptorDictionary(builder, mod, descriptorPtr);
		//	//auto lookupfun = RTDictionaryLookup::Instance().GetLLVMElement(mod);
		//	//auto lookupcall = builder->CreateCall(lookupfun, { dictptr, key });
		//	//lookupcall->setCallingConv(lookupfun->getCallingConv());
		//	//auto entry = builder->CreatePointerCast(lookupcall, GetDescriptorDictionaryEntryType()->getPointerTo());
		//	//auto entryIsNull = GenerateIsNull(builder, entry);
		//	//builder->CreateCondBr(entryIsNull, nullBlock, notnullBlock);
		//}
		//llvm::Value* RTPartialApp::CreateDescriptorDictionaryLookup(NomBuilder& builder, llvm::Module& mod, NomValue receiver, llvm::Value* key, const llvm::Twine keyname)
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

		//	Function* fun = builder->GetInsertBlock()->getParent();
		//	BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "isRef", fun);
		//	BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "isInt", fun);
		//	BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "isFloat", fun);
		//	BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "dictLookup", fun);

		//	auto typeKind = builder->CreateTrunc(builder->CreatePtrToInt(receiver, numtype(intptr_t)), IntegerType::get(LLVMCONTEXT, 2));
		//	auto kindSwitch = builder->CreateSwitch(typeKind, floatBlock, 2);
		//	kindSwitch->addCase(ConstantInt::get(IntegerType::get(LLVMCONTEXT, 2), 0), refBlock);
		//	kindSwitch->addCase(ConstantInt::get(IntegerType::get(LLVMCONTEXT, 2), 1), intBlock);

		//	builder->SetInsertPoint(refBlock);
		//	auto descriptor = MakeLoad(builder, mod, builder->CreateGEP(receiver, { MakeInt32(0), MakeInt32(ObjectHeaderFields::ClassDescriptor) }));
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(intBlock);
		//	auto intDescriptor = NomIntClass::GetInstance()->GetLLVMElement(mod);
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(floatBlock);
		//	auto floatDescriptor = NomFloatClass::GetInstance()->GetLLVMElement(mod);
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(mergeBlock);
		//	auto descPHI = builder->CreatePHI(descriptor->getType(), 3, "descriptor");
		//	descPHI->addIncoming(descriptor, refBlock);
		//	descPHI->addIncoming(intDescriptor, intBlock);
		//	descPHI->addIncoming(floatDescriptor, floatBlock);
		//	auto dictionary = GenerateReadDescriptorDictionary(builder, mod, builder->CreatePointerCast(descPHI, GetLLVMType()->getPointerTo()));
		//	auto lookupfun = RTDictionaryLookup::Instance().GetLLVMElement(mod);
		//	auto lookupcall = builder->CreateCall(lookupfun, { dictionary, key }, (keyname.isTriviallyEmpty() ? "" : ("Lookup " + keyname)));
		//	lookupcall->setCallingConv(lookupfun->getCallingConv());
		//	auto entry = builder->CreatePointerCast(lookupcall, GetDescriptorDictionaryEntryType()->getPointerTo());
		//	return entry;
		//}
		//llvm::Value* RTPartialApp::CreateDictionaryLoad(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key, const llvm::Twine keyname)
		//{
		//	BasicBlock* nullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* notnullBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* refValueBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* dispatcherBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	BasicBlock* partialAppBlock = BasicBlock::Create(LLVMCONTEXT, "", builder->GetInsertBlock()->getParent());
		//	auto dictptr = GenerateReadDescriptorDictionary(builder, mod, cdesc);
		//	auto lookupfun = RTDictionaryLookup::Instance().GetLLVMElement(mod);
		//	auto lookupcall = builder->CreateCall(lookupfun, { dictptr, key });
		//	lookupcall->setCallingConv(lookupfun->getCallingConv());
		//	auto entry = builder->CreatePointerCast(lookupcall, GetDescriptorDictionaryEntryType()->getPointerTo());
		//	auto entryIsNull = GenerateIsNull(builder, entry);
		//	builder->CreateCondBr(entryIsNull, nullBlock, notnullBlock);

		//	builder->SetInsertPoint(nullBlock);
		//	auto nullval = ConstantPointerNull::get(POINTERTYPE);
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(notnullBlock);
		//	auto kind = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::Kind) }));
		//	auto kindSwitch = builder->CreateSwitch(kind, refValueBlock, 2);
		//	kindSwitch->addCase(MakeInt<DescriptorDictionaryEntryKind>(DescriptorDictionaryEntryKind::Dispatcher), dispatcherBlock);
		//	kindSwitch->addCase(MakeInt<DescriptorDictionaryEntryKind>(DescriptorDictionaryEntryKind::PartialApp), partialAppBlock);

		//	builder->SetInsertPoint(dispatcherBlock);
		//	auto dispatcher = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::TypeOrPartialAppOrDispatcher) }));
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(partialAppBlock);
		//	auto partialApp = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::TypeOrPartialAppOrDispatcher) }));
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(refValueBlock);
		//	auto index = MakeLoad(builder, mod, builder->CreateGEP(entry, { MakeInt32(0), MakeInt32(DescriptorDictionaryEntryFields::Index) }));
		//	llvm::Value* fieldptr = builder->CreateGEP(thisObj, { MakeInt32(0), MakeInt32((unsigned char)ObjectHeaderFields::Fields), builder->CreateNeg(builder->CreateAdd(index, MakeInt32(1))) });
		//	LoadInst* loadinst = MakeLoad(builder, mod, fieldptr);
		//	auto fieldValue = builder->CreatePointerCast(loadinst, POINTERTYPE);
		//	builder->CreateBr(mergeBlock);

		//	builder->SetInsertPoint(mergeBlock);
		//	auto phi = builder->CreatePHI(POINTERTYPE, 4);
		//	phi->addIncoming(nullval, nullBlock);
		//	phi->addIncoming(partialApp, partialAppBlock);
		//	phi->addIncoming(dispatcher, dispatcherBlock);
		//	phi->addIncoming(fieldValue, refValueBlock);
		//	return phi;
		//}
		//llvm::Value* RTPartialApp::CreateDictionaryStore(NomBuilder& builder, llvm::Module& mod, llvm::Value* thisObj, llvm::Value* cdesc, llvm::ConstantInt* key, llvm::Value* value)
		//{
		//	return nullptr;
		//}
	}
}