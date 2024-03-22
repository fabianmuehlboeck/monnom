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
PUSHDIAGSUPPRESSION
#include "llvm/ADT/SmallSet.h"
POPDIAGSUPPRESSION
#include "CastStats.h"
#include "RTLambda.h"
#include "LambdaHeader.h"
#include "IMT.h"
#include "Metadata.h"
#include "PWRefValue.h"
#include "PWDispatchPair.h"
#include "PWAll.h"

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


		llvm::Value* EnsureDynamicMethodInstruction::GenerateGetBestInvokeDispatcherDyn(NomBuilder& builder, RTValuePtr receiver)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			BasicBlock* retBlock = nullptr;
			llvm::Value* retVal = nullptr;

			receiver->GenerateRefOrPrimitiveValueSwitch(builder,
				[fun, receiver, &retBlock, &retVal](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWRefValue> val) -> void {
					BasicBlock* packPairBlock = BasicBlock::Create(LLVMCONTEXT, "packRawInvokeDispatcherPair", fun);
					BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(b, "Given value is not invokable!");
					PWRefValue rv = PWRefValue(receiver);
					auto vtable = rv.ReadVTable(b);
					auto hasRawInvoke = vtable.ReadHasRawInvoke(b);
					b->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { hasRawInvoke, MakeUInt(1,1) });
					b->CreateCondBr(hasRawInvoke, packPairBlock, errorBlock, GetLikelyFirstBranchMetadata());

					b->SetInsertPoint(packPairBlock);
					auto rawInvokePtr = RefValueHeader::GenerateReadRawInvoke(b, receiver);
					retVal = PWDispatchPair::Get(b, rawInvokePtr, receiver);
					retBlock = b->GetInsertBlock();
				},
				[](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWPacked> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Cannot invoke integer values!", b->GetInsertBlock());
				},
				[](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWPacked> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Cannot invoke float values!", b->GetInsertBlock());
				},
				[](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWInt64> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Cannot invoke integer values!", b->GetInsertBlock());
				},
				[](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWFloat> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Cannot invoke float values!", b->GetInsertBlock());
				},
				[](NomBuilder& b, [[maybe_unused]] RTPWValuePtr<PWBool> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Cannot invoke boolean values!", b->GetInsertBlock());
				}, 200, 10, 10);

			if (retBlock == nullptr)
			{
				throw new std::exception();
			}

			builder->SetInsertPoint(retBlock);
			return retVal;
		}

		void EnsureDynamicMethodInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			RTValuePtr receiver = (*env)[Receiver];
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

			BasicBlock** retBlocks = makealloca(BasicBlock*, 6);
			PWDispatchPair* retValues = makealloca(PWDispatchPair, 6);
			unsigned int retCount = 0;

			receiver->GenerateRefOrPrimitiveValueSwitch(builder,
				[&methodName, retBlocks, retValues, &retCount](NomBuilder& b, RTPWValuePtr<PWRefValue> val) -> void {
					auto vtable = val->value.ReadVTable(b);
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val, vtable, NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& b, RTPWValuePtr<PWPacked> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val, NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& b, RTPWValuePtr<PWPacked> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val, NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++; 
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& b, RTPWValuePtr<PWInt64> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val->AsPackedValue(b) , NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& b, RTPWValuePtr<PWFloat> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val->AsPackedValue(b), NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& b, RTPWValuePtr<PWBool> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(b, val->AsPackedValue(b), NomBoolClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = b->GetInsertBlock();
					retCount++;
				},
				100, 20, 10);
			
			if (retCount == 0)
			{
				throw new std::exception();
			}
			if (retCount > 1)
			{
				BasicBlock* mergeBlock = BasicBlock::Create(builder->getContext(), "ensureDynDispatcherMerge", fun);
				builder->SetInsertPoint(mergeBlock);
				PWPhi<PWDispatchPair> phi = PWPhi<PWDispatchPair>::Create(builder, retCount, "dynDispatcherPairPHI");
				for (unsigned int i = 0; i < retCount; i++)
				{
					builder->SetInsertPoint(retBlocks[i]);
					builder->CreateBr(mergeBlock);
					phi->addIncoming(retValues[i], retBlocks[i]);
				}
				builder->SetInsertPoint(mergeBlock);
			}
			else
			{
				builder->SetInsertPoint(retBlocks[0]);
				env->PushDispatchPair(retValues[0]);
			}
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
