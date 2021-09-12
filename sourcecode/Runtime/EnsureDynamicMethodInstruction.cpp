#include "EnsureDynamicMethodInstruction.h"
#include <stdio.h>
#include "NomConstants.h"
#include "RefValueHeader.h"
#include "CompileEnv.h"
#include "RTInterface.h"
#include "RTVTable.h"
#include "NomNameRepository.h"
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "CallingConvConf.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "NomMethod.h"
#include "NomMethodTableEntry.h"
#include "llvm/ADT/SmallSet.h"
#include "CastStats.h"
#include "RTLambda.h"
#include "LambdaHeader.h"
#include "IMT.h"
#include "Metadata.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		EnsureDynamicMethodInstruction::EnsureDynamicMethodInstruction(ConstantID methodNameID, RegIndex receiver) : NomInstruction(OpCode::EnsureDynamicMethod), MethodName(methodNameID), Receiver(receiver)
		{
		}
		EnsureDynamicMethodInstruction::~EnsureDynamicMethodInstruction()
		{
		}


		llvm::Value* EnsureDynamicMethodInstruction::GenerateGetBestInvokeDispatcherDyn(NomBuilder& builder, NomValue receiver)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			int mergeBlocks = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "ddLookupMerge", fun);
			PHINode* mergePHI = nullptr;
			Value* returnVal = nullptr;

			if (mergeBlocks == 0)
			{
				throw new std::exception();
			}
			if (mergeBlocks > 1)
			{
				builder->SetInsertPoint(mergeBlock);
				mergePHI = builder->CreatePHI(GetDynamicDispatcherLookupResultType(), mergeBlocks);
				returnVal = mergePHI;
			}

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				BasicBlock* packPairBlock = BasicBlock::Create(LLVMCONTEXT, "packRawInvokeDispatcherPair", fun);
				BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Given value is not invokable!");
				auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				auto hasRawInvoke = RTVTable::GenerateHasRawInvoke(builder, vtable);
				builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { hasRawInvoke, MakeUInt(1,1) });
				builder->CreateCondBr(hasRawInvoke, packPairBlock, errorBlock, GetLikelyFirstBranchMetadata());

				builder->SetInsertPoint(packPairBlock);
				auto rawInvokePtr = builder->CreatePointerCast(RefValueHeader::GenerateReadRawInvoke(builder, receiver), GetIMTFunctionType()->getPointerTo());
				auto partialPair = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), rawInvokePtr, { 0 });
				auto returnVal2 = builder->CreateInsertValue(partialPair, builder->CreatePointerCast(receiver, POINTERTYPE), { 1 });

				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(returnVal2, builder->GetInsertBlock());
				}
				else
				{
					returnVal = returnVal2;
				}
				builder->CreateBr(mergeBlock);
			}

			if (packedIntBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke integer values!", packedIntBlock);
			}
			if (packedFloatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke float values!", packedFloatBlock);
			}
			if (primitiveIntBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke integer values!", primitiveIntBlock);
			}
			if (primitiveFloatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke float values!", primitiveFloatBlock);
			}
			if (primitiveBoolBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke boolean values!", primitiveBoolBlock);
			}

			builder->SetInsertPoint(mergeBlock);
			return returnVal;
		}

		void EnsureDynamicMethodInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			NomValue receiver = (*env)[Receiver];
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();

			auto methodName = NomConstants::GetString(MethodName)->GetText()->ToStdString();

			if (methodName.empty())
			{
				if (NomCastStats)
				{
					builder->CreateCall(GetIncDynamicInvokes(*builder->GetInsertBlock()->getParent()->getParent()), {});
				}
				env->PushDispatchPair(GenerateGetBestInvokeDispatcherDyn(builder, receiver));
				return;
			}

			if (NomCastStats)
			{
				builder->CreateCall(GetIncDynamicMethodCalls(*builder->GetInsertBlock()->getParent()->getParent()), {});
			}

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			Value* primitiveIntVal, * primitiveFloatVal, * primitiveBoolVal;

			int mergeBlocks = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, &primitiveIntVal, &primitiveFloatBlock, &primitiveFloatVal, &primitiveBoolBlock, &primitiveBoolVal);

			BasicBlock* mergeBlock = BasicBlock::Create(LLVMCONTEXT, "ddLookupMerge", fun);
			PHINode* mergePHI = nullptr;
			Value* returnVal = nullptr;

			if (mergeBlocks == 0)
			{
				throw new std::exception();
			}
			if (mergeBlocks > 1)
			{
				builder->SetInsertPoint(mergeBlock);
				mergePHI = builder->CreatePHI(GetDynamicDispatcherLookupResultType(), mergeBlocks);
				returnVal = mergePHI;
			}

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
				auto returnVal2 = RTVTable::GenerateFindDynamicDispatcherPair(builder, receiver, vtable, NomNameRepository::Instance().GetNameID(methodName));

				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(returnVal2, builder->GetInsertBlock());
				}
				else
				{
					returnVal = returnVal2;
				}
				builder->CreateBr(mergeBlock);
			}

			if (packedIntBlock != nullptr)
			{
				builder->SetInsertPoint(packedIntBlock);
				auto intDispatcher = RTVTable::GenerateFindDynamicDispatcherPair(builder, receiver, NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(intDispatcher, builder->GetInsertBlock());
				}
				else
				{
					returnVal = intDispatcher;
				}
				builder->CreateBr(mergeBlock);

			}
			if (packedFloatBlock != nullptr)
			{
				builder->SetInsertPoint(packedFloatBlock);
				auto floatDispatcher = RTVTable::GenerateFindDynamicDispatcherPair(builder, receiver, NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(floatDispatcher, builder->GetInsertBlock());
				}
				else
				{
					returnVal = floatDispatcher;
				}
				builder->CreateBr(mergeBlock);
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				auto packedInt = PackInt(builder, receiver);
				auto intDispatcher = RTVTable::GenerateFindDynamicDispatcherPair(builder, packedInt, NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(intDispatcher, builder->GetInsertBlock());
				}
				else
				{
					returnVal = intDispatcher;
				}
				builder->CreateBr(mergeBlock);
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				auto packedFloat = PackFloat(builder, receiver);
				auto floatDispatcher = RTVTable::GenerateFindDynamicDispatcherPair(builder, packedFloat, NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(floatDispatcher, builder->GetInsertBlock());
				}
				else
				{
					returnVal = floatDispatcher;
				}
				builder->CreateBr(mergeBlock);
			}
			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				auto packedBool = PackBool(builder, receiver);
				auto boolDispatcher = RTVTable::GenerateFindDynamicDispatcherPair(builder, packedBool, NomBoolClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
				if (mergePHI != nullptr)
				{
					mergePHI->addIncoming(boolDispatcher, builder->GetInsertBlock());
				}
				else
				{
					returnVal = boolDispatcher;
				}
				builder->CreateBr(mergeBlock);
			}

			builder->SetInsertPoint(mergeBlock);
			env->PushDispatchPair(returnVal);
		}
		void EnsureDynamicMethodInstruction::Print(bool resolve)
		{
			cout << "EnsureDynamicMethod ";
			NomConstants::PrintConstant(MethodName, resolve);
			cout << "@" << Receiver;
			cout << "\n";
		}
		void EnsureDynamicMethodInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(MethodName);
		}
	}
}