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
				[fun, receiver, &retBlock, &retVal](NomBuilder& builder, RTPWValuePtr<PWRefValue> val) -> void {
					BasicBlock* packPairBlock = BasicBlock::Create(LLVMCONTEXT, "packRawInvokeDispatcherPair", fun);
					BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Given value is not invokable!");
					PWRefValue rv = PWRefValue(receiver);
					auto vtable = rv.ReadVTable(builder);
					auto hasRawInvoke = vtable.ReadHasRawInvoke(builder);
					builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { hasRawInvoke, MakeUInt(1,1) });
					builder->CreateCondBr(hasRawInvoke, packPairBlock, errorBlock, GetLikelyFirstBranchMetadata());

					builder->SetInsertPoint(packPairBlock);
					auto rawInvokePtr = RefValueHeader::GenerateReadRawInvoke(builder, receiver);
					retVal = PWDispatchPair::Get(builder, rawInvokePtr, receiver);
					retBlock = builder->GetInsertBlock();
				},
				[](NomBuilder& builder, RTPWValuePtr<PWPacked> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke integer values!", builder->GetInsertBlock());
				},
				[](NomBuilder& builder, RTPWValuePtr<PWPacked> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke float values!", builder->GetInsertBlock());
				},
				[](NomBuilder& builder, RTPWValuePtr<PWInt64> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke integer values!", builder->GetInsertBlock());
				},
				[](NomBuilder& builder, RTPWValuePtr<PWFloat> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke float values!", builder->GetInsertBlock());
				},
				[](NomBuilder& builder, RTPWValuePtr<PWBool> val) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Cannot invoke boolean values!", builder->GetInsertBlock());
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
				[&methodName, retBlocks, retValues, &retCount](NomBuilder& builder, RTPWValuePtr<PWRefValue> val) -> void {
					auto vtable = val->value.ReadVTable(builder);
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val, vtable, NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& builder, RTPWValuePtr<PWPacked> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val, NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& builder, RTPWValuePtr<PWPacked> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val, NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
					retCount++; 
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& builder, RTPWValuePtr<PWInt64> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val->AsPackedValue(builder) , NomIntClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& builder, RTPWValuePtr<PWFloat> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val->AsPackedValue(builder), NomFloatClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
					retCount++;
				},
				[&methodName, retBlocks, retValues, &retCount, env](NomBuilder& builder, RTPWValuePtr<PWBool> val) -> void {
					retValues[retCount] = RTVTable::GenerateFindDynamicDispatcherPair(builder, val->AsPackedValue(builder), NomBoolClass::GetInstance()->GetLLVMElement(*env->Module), NomNameRepository::Instance().GetNameID(methodName));
					retBlocks[retCount] = builder->GetInsertBlock();
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
