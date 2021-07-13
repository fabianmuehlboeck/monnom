#include "RTMulticastedInterface.h"
#include "RTInterface.h"
#include "RTGeneralInterface.h"
#include "IMT.h"
#include "RTClass.h"
#include "CallingConvConf.h"
#include "RTCast.h"
#include "NomVMInterface.h"
#include "CompileHelpers.h"
#include "NomInterface.h"
#include "NomTypeRegistry.h"
#include "RTSubtyping.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		llvm::StructType* RTMulticastedInterface::GetLLVMType()
		{
			static auto sit = StructType::create(LLVMCONTEXT, "RT_NOM_MulticastInterface");
			static bool once = true;
			if (once)
			{
				once = false;
				sit->setBody(RTClass::GetLLVMType()		//regular class stuff
				);
			}
			return sit;
		}


		llvm::Constant* RTMulticastedInterface::CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage)
		{
			return nullptr;
		}
		//llvm::StructType* RTMulticastVTable::GetLLVMType()
		//{
		//	static StructType* rtitt = StructType::create(LLVMCONTEXT, "RT_NOM_MulticastVTable");
		//	static bool once = true;
		//	if (once)
		//	{
		//		once = false;
		//		rtitt->setBody(
		//			RTVTable::GetLLVMType(),			//Original VTable Stuff
		//			arrtype(POINTERTYPE, 0),			//Method table, goes up
		//			numtype(intptr_t),											//interface entries offset (odd) / pointer to next struct interface list entry (even)
		//			numtype(RTDescriptorKind),									//Kind
		//			POINTERTYPE													//Nom IR descriptor
		//		);
		//	}
		//	return rtitt;
		//}
		llvm::FunctionType* RTMulticast::GetLLVMType()
		{
			static FunctionType* ft = FunctionType::get(RTVTable::GetLLVMType()->getPointerTo(), { RTClass::GetLLVMType()->getPointerTo(), TYPETYPE, RTSubtyping::TypeArgumentListStackType()->getPointerTo() }, false);
			return ft;
		}
		llvm::Function* RTMulticast::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			static std::string printSymName = "RT_NOM_Multicast_AddInterface";
			llvm::Function* fun = Function::Create(GetLLVMType(), linkage, printSymName, &mod);
			fun->setCallingConv(NOMCC);

			NomBuilder builder;

			auto argiter = fun->arg_begin();
			Argument* cls = argiter;  // RTClass^
			argiter++;
			Argument* target = argiter; // RTTypeHead^
			argiter++;
			Argument* substStack = argiter; // {next^, RTTypeHead^}^
			argiter++;

			BasicBlock* startBlock = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$start", fun);
			BasicBlock* origSuperLoopHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$origSupersLoop$head", fun);
			BasicBlock* checkNewType = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$checkNewType", fun);
			BasicBlock* validNewType = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$validNewType", fun);
			BasicBlock* newSuperLoopHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$newSupersLoop$head", fun);
			BasicBlock* sumCountsBlock = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$sumCounts", fun);
			BasicBlock* invalidNewType = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$invalidNewType", fun);
			BasicBlock* copyOrigSupersLoopHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyOrigSupersLoop$head", fun);
			BasicBlock* copyNewSuper = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSuper", fun);
			BasicBlock* copyNewSupersLoopHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSupersLoop$head", fun);
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$finish", fun);

			//Step 1: count super classes and total number of type arguments in existing class and cast target type
			//  Note: this may over-count the actual number of entries at the end because some types may occur in 
			//        both; but we'll do that check only when transferring the entries after allocating space

			builder->SetInsertPoint(startBlock);

			llvm::Value* orig_supercount = EnsureIntegerSize(builder, RTClass::GenerateReadSuperInstanceCount(builder, cls), 32);
			llvm::Value* orig_supers = RTClass::GenerateReadSuperInstances(builder, cls); // {RTInterface^, RTTypeHead^}^
			llvm::Value* orig_typeargcount;

			builder->CreateBr(origSuperLoopHead);

			{
				BasicBlock* origSuperLoopBody = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$origSupersLoop$body", fun);

				//origSuperLoopHead
				builder->SetInsertPoint(origSuperLoopHead);
				auto superCountPHI = builder->CreatePHI(orig_supercount->getType(), 2, "superCountPHI");
				auto typeargCountPHI = builder->CreatePHI(orig_supercount->getType(), 2, "typeArgCountPHI");
				superCountPHI->addIncoming(orig_supercount, startBlock);
				typeargCountPHI->addIncoming(MakeIntLike(orig_supercount, 0), startBlock);

				Value* noRemainingSupers = builder->CreateICmpEQ(superCountPHI, MakeIntLike(superCountPHI, 0));
				builder->CreateCondBr(noRemainingSupers, checkNewType, origSuperLoopBody);

				//origSuperLoopBody
				builder->SetInsertPoint(origSuperLoopBody);
				auto superOffset = builder->CreateSub(superCountPHI, MakeIntLike(superCountPHI, 1));
				superCountPHI->addIncoming(superOffset, origSuperLoopBody);
				auto superInterface = MakeLoad(builder, builder->CreateGEP(orig_supers, { superOffset, 0 }, "origSuperEntryPtr"), "origSuperEntry"); //RTInterface^
				auto superInterfaceTargcount = RTInterface::GenerateReadTypeArgCount(builder, superInterface);
				auto totalTargcount = builder->CreateAdd(typeargCountPHI, superInterfaceTargcount, "targcountSum");
				typeargCountPHI->addIncoming(totalTargcount, origSuperLoopBody);

				builder->CreateBr(origSuperLoopHead);

				orig_typeargcount = typeargCountPHI;
			}

			//checkNewType
			builder->SetInsertPoint(checkNewType);

			Value* newTypeKind = RTTypeHead::GenerateReadTypeKind(builder, target);
			SwitchInst* newTypeKindSwitch = builder->CreateSwitch(newTypeKind, invalidNewType, 2);
			newTypeKindSwitch->addCase(MakeInt(TypeKind::TKClass), validNewType);

			//validNewType
			builder->SetInsertPoint(validNewType);
			Value* newcls = RTClassType::GenerateReadClassDescriptorLink(builder, target);
			llvm::Value* new_supercount = RTClass::GenerateReadSuperInstanceCount(builder, newcls);
			llvm::Value* new_supers = RTClass::GenerateReadSuperInstances(builder, newcls); // {RTInterface^, RTTypeHead^}^
			llvm::Value* new_typeargcount = RTClass::GenerateReadTypeArgCount(builder, newcls);

			builder->CreateBr(newSuperLoopHead);

			{
				BasicBlock* newSuperLoopBody = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$newSupersLoop$body", fun);

				//newSuperLoopHead
				builder->SetInsertPoint(newSuperLoopHead);
				auto superCountPHI = builder->CreatePHI(new_supercount->getType(), 2, "superCountPHI");
				auto typeargCountPHI = builder->CreatePHI(new_supercount->getType(), 2, "typeArgCountPHI");
				superCountPHI->addIncoming(new_supercount, validNewType);
				typeargCountPHI->addIncoming(new_typeargcount, validNewType);

				Value* noRemainingSupers = builder->CreateICmpEQ(superCountPHI, MakeIntLike(superCountPHI, 0));
				builder->CreateCondBr(noRemainingSupers, sumCountsBlock, newSuperLoopBody);

				//newSuperLoopBody
				builder->SetInsertPoint(newSuperLoopBody);
				auto superOffset = builder->CreateSub(superCountPHI, MakeIntLike(superCountPHI, 1));
				superCountPHI->addIncoming(superOffset, newSuperLoopBody);
				auto superInterface = MakeLoad(builder, builder->CreateGEP(new_supers, { superOffset, 0 }, "origSuperEntryPtr"), "origSuperEntry"); //RTInterface^
				auto superInterfaceTargcount = RTInterface::GenerateReadTypeArgCount(builder, superInterface);
				auto totalTargcount = builder->CreateAdd(typeargCountPHI, superInterfaceTargcount, "targcountSum");
				typeargCountPHI->addIncoming(totalTargcount, newSuperLoopBody);

				builder->CreateBr(newSuperLoopHead);

				new_typeargcount = typeargCountPHI;
			}

			//sumCountsBlock
			builder->SetInsertPoint(sumCountsBlock);

			llvm::Value* supercount = builder->CreateAdd(builder->CreateAdd(orig_supercount, new_supercount), MakeIntLike(orig_supercount, 1), "totalSuperCount"); //+1 for target type itself
			llvm::Value* argscount = builder->CreateAdd(orig_typeargcount, new_typeargcount, "totalTargcount");

			llvm::Function* gcallocfun = GetAlloc(&mod); //gcmalloc

			StructType* allocType = StructType::get(LLVMCONTEXT, { POINTERTYPE, RTClass::GetLLVMType(), arrtype(SuperInstanceEntryType(), 0) });
			llvm::Value* size1 = MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(allocType));
			llvm::Value* size2 = builder->CreateMul(MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(SuperInstanceEntryType())), supercount);
			Value* size3 = builder->CreateMul(MakeInt<int64_t>(GetNomJITDataLayout().getTypeAllocSize(TYPETYPE)), argscount);

			Value* size23 = builder->CreateAdd(size2, size3);

			llvm::Value* size = builder->CreateAdd(size1, size23, "allocSize");

			llvm::Value* allocmem = builder->CreateCall(gcallocfun, { size }, "newmem");
			llvm::Value* allochead = builder->CreatePointerCast(allocmem, allocType);

			Function* imt = RTMulticastIMT::Instance().GetLLVMElement(mod);

			MakeStore(builder, imt, allochead, MakeInt32(0));

			Value* result = builder->CreateGEP(allochead, { MakeInt32(0), MakeInt32(1) });
			Value* superentries = builder->CreateGEP(allochead, { MakeInt32(0), MakeInt32(2) });
			Value* typeargstart = builder->CreateGEP(allochead, { MakeInt32(0), MakeInt32(2), builder->CreateTrunc(supercount, inttype(32)) });
			Value* fieldlookup = MakeLoad(builder, cls, MakeInt32(RTClassFields::FieldLookup), "fieldlookup");
			Value* fieldstore = MakeLoad(builder, cls, MakeInt32(RTClassFields::FieldStore), "fieldstore");
			Value* fieldcount = MakeLoad(builder, cls, MakeInt32(RTClassFields::FieldCount), "fieldcount");
			Value* displookup = MakeLoad(builder, cls, MakeInt32(RTClassFields::DispatcherLookup), "dispLookup");

			builder->CreateBr(copyOrigSupersLoopHead);

			{

				BasicBlock* copyOrigSupersLoopBody = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyOrigSupersLoop$body", fun);

				//copyOrigSupersLoopHead
				builder->SetInsertPoint(copyOrigSupersLoopHead);
				PHINode* superInsertCounterPHI = builder->CreatePHI(inttype(32), 2);
				PHINode* typeargsposPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2);
				superInsertCounterPHI->addIncoming(MakeInt32(0), sumCountsBlock);
				typeargsposPHI->addIncoming(builder->CreatePointerCast(typeargstart, TYPETYPE->getPointerTo()), sumCountsBlock);

				Value* remainingSupers = builder->CreateICmpULT(superInsertCounterPHI, orig_supercount, "supersRemaining");

				builder->CreateCondBr(remainingSupers, copyOrigSupersLoopBody, copyNewSuper);

				//copyOrigSupersLoopBody
				builder->SetInsertPoint(copyOrigSupersLoopBody);
				
				Value* superInterface = MakeLoad(builder, builder->CreateGEP(orig_supers, { superInsertCounterPHI, MakeInt32(SuperInstanceEntryFields::Class) }), "origSuperIface");
				Value* superArgs = MakeLoad(builder, builder->CreateGEP(orig_supers, { superInsertCounterPHI, MakeInt32(SuperInstanceEntryFields::TypeArgs) }), "superSuperTypeArgs");
				Value* superArgCount = RTInterface::GenerateReadTypeArgCount(builder, superInterface);


				builder->CreateMemCpyInline(typeargsposPHI, llvm::MaybeAlign(8), superArgs, llvm::MaybeAlign(8), EnsureIntegerSize(builder, builder->CreateMul(superArgCount, MakeIntLike(superArgCount, GetNomJITDataLayout().getTypeAllocSize(TYPETYPE))), 32));

				MakeStore(builder, superInterface, builder->CreateGEP(superentries, { superInsertCounterPHI, MakeInt32(SuperInstanceEntryFields::Class) }));
				MakeStore(builder, typeargsposPHI, builder->CreateGEP(superentries, { superInsertCounterPHI, MakeInt32(SuperInstanceEntryFields::TypeArgs) }));


				Value* nexttargpos = builder->CreateGEP(typeargsposPHI, EnsureIntegerSize(builder, superArgCount, 32));
				typeargsposPHI->addIncoming(nexttargpos, copyOrigSupersLoopBody);
				superInsertCounterPHI->addIncoming(builder->CreateAdd(superInsertCounterPHI, MakeIntLike(superInsertCounterPHI, 1)), copyOrigSupersLoopBody);
				builder->CreateBr(copyOrigSupersLoopHead);
				//builder->CreateBr(copyTypeArgsLoopHead);
				//{

				//	BasicBlock* copyTypeArgsLoopBody = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyOrigSupersLoop$copyTypeArgsLoop$body", fun);
				//	//copyTypeArgsLoopHead
				//	builder->SetInsertPoint(copyTypeArgsLoopHead);
				//	PHINode* superTypeArgsCountPHI = builder->CreatePHI(superArgCount->getType(), 2);
				//	superTypeArgsCountPHI->addIncoming(superArgCount, copyOrigSupersLoopBody);
				//	Value* moMoreArgsToCopy = builder->CreateICmpEQ(superTypeArgsCountPHI, MakeIntLike(superTypeArgsCountPHI, 0));
				//	builder->CreateCondBr(moMoreArgsToCopy, copyNewSupersLoopHead, copyTypeArgsLoopBody);

				//	//copyTypeArgsLoopBody
				//	builder->SetInsertPoint(copyTypeArgsLoopBody);


				//	builder->CreateBr(copyTypeArgsLoopHead);
				//}
				typeargstart = typeargsposPHI;
			}

			//insert cast target
			builder->SetInsertPoint(copyNewSuper);
			MakeStore(builder, newcls, builder->CreateGEP(superentries, { EnsureIntegerSize(builder, orig_supercount, 32), MakeInt32(SuperInstanceEntryFields::Class) }));
			MakeStore(builder, typeargstart, builder->CreateGEP(superentries, { EnsureIntegerSize(builder, orig_supercount, 32), MakeInt32(SuperInstanceEntryFields::TypeArgs) }));

			
			{
				BasicBlock* copyNewSuperArgsHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSuper$args$head", fun);
				BasicBlock* copyNewSuperArgsLoop = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSuper$args$loop", fun);

				llvm::Value* targetTargs = RTClassType::GetTypeArgumentsPtr(builder, target);
				llvm::Value* targcount = RTInterface::GenerateReadTypeArgCount(builder, newcls);

				builder->CreateBr(copyNewSuperArgsHead);

				builder->SetInsertPoint(copyNewSuperArgsHead);
				PHINode* typeargscountPHI = builder->CreatePHI(inttype(32), 2);
				typeargscountPHI->addIncoming(MakeInt32(0), copyNewSuper);

				llvm::Value* moreTypeArgs = builder->CreateICmpULT(typeargscountPHI, targcount);

				builder->CreateCondBr(moreTypeArgs, copyNewSuperArgsLoop, copyNewSupersLoopHead);

				builder->SetInsertPoint(copyNewSuperArgsLoop);

				auto writeType = MakeLoad(builder, builder->CreateGEP(targetTargs, typeargscountPHI));
				auto uniquedType = builder->CreateCall(TypeRegistry::Instance().GetLLVMElement(mod), { writeType, substStack }, "uniquedType");
				uniquedType->setCallingConv(NOMCC);
				MakeStore(builder, uniquedType, builder->CreateGEP(typeargstart, typeargscountPHI));

				builder->CreateBr(copyNewSuperArgsHead);
			}

			llvm::Value* newSupersIndex = builder->CreateAdd(EnsureIntegerSize(builder, orig_supercount, 32), MakeInt32(1));
			typeargstart = builder->CreateAdd(typeargstart, MakeInt32(1));

			{
				BasicBlock* copyNewSupersLoopBody = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSupersLoop$body", fun);
				BasicBlock* copyNewSupersLoopCheckHead = BasicBlock::Create(LLVMCONTEXT, "RT_NOM_Multicast_AddInterface$copyNewSupersLoop$body", fun);

				builder->SetInsertPoint(copyNewSupersLoopHead);
				PHINode* superInsertCounterPHI = builder->CreatePHI(inttype(32), 2);
				superInsertCounterPHI->addIncoming(MakeInt32(0), copyOrigSupersLoopHead);
				PHINode* typeargsposPHI = builder->CreatePHI(TYPETYPE->getPointerTo(), 2);
				typeargsposPHI->addIncoming(typeargstart, copyOrigSupersLoopHead);
				
				//BasicBlock* 

			}

			//outBlock
			builder->SetInsertPoint(outBlock);

			//Note: when sharing generated class descriptors, it may be useful to give them type parameters that they instantiate their interfaces with
			RTClass::GenerateInitialization(builder, result, MakeInt<intptr_t>(0), MakeInt(RTDescriptorKind::Class), ConstantPointerNull::get(POINTERTYPE), MakeInt<RTInterfaceFlags>(RTInterfaceFlags::None), 0, supercount, superentries, fieldlookup, fieldstore, displookup, fieldcount);

			builder->CreateRet(result);
		}
		llvm::Function* RTMulticast::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_Multicast_AddInterface");
		}
		llvm::Function* RTMulticastIMT::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			return nullptr;
		}
		llvm::Function* RTMulticastIMT::findLLVMElement(llvm::Module& mod) const
		{
			return nullptr;
		}
		RTMulticastIMT& RTMulticastIMT::Instance()
		{
			static RTMulticastIMT rts; return rts;
		}
	}
}