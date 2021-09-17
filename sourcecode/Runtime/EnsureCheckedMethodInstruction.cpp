#include "EnsureCheckedMethodInstruction.h"
#include <stdio.h>
#include "NomConstants.h"
#include "RefValueHeader.h"
#include "CompileEnv.h"
#include "NomClassType.h"
#include "RTOutput.h"
#include "RTRecord.h"
#include "NomNameRepository.h"
#include "CompileHelpers.h"
#include "CallingConvConf.h"
#include "RTVTable.h"
#include "RTCompileConfig.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		EnsureCheckedMethodInstruction::EnsureCheckedMethodInstruction(ConstantID methodNameID, RegIndex receiver) : NomInstruction(OpCode::EnsureCheckedMethod), MethodName(methodNameID), Receiver(receiver)
		{
		}
		EnsureCheckedMethodInstruction::~EnsureCheckedMethodInstruction()
		{
		}
		void EnsureCheckedMethodInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			//auto methodName = NomConstants::GetString(MethodName)->GetText()->ToStdString();
			//if (((!RTConfig_IgnoreEnsureMethod)&&!methodName.empty())|| RTConfig_AlwaysEnsureMethod)
			//{
			//	BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			//	auto receiver = (*env)[Receiver];
			//	int cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);
			//	BasicBlock* returnBlock = nullptr;
			//	if (cases > 1)
			//	{
			//		returnBlock = BasicBlock::Create(LLVMCONTEXT, "ensureMethodOut", builder->GetInsertBlock()->getParent());
			//	}

			//	if (refValueBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(refValueBlock);
			//		auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, receiver);
			//		auto nameid = NomNameRepository::Instance().GetNameID(methodName);
			//		RTVTable::GenerateFreezeMethodField(builder, receiver, vtable, MakeInt<size_t>(nameid), MakeUInt(32, nameid % IMTsize));
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}

			//	if (intBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(intBlock);
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}
			//	if (floatBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(floatBlock);
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}
			//	if (primitiveIntBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(primitiveIntBlock);
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}
			//	if (primitiveFloatBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(primitiveFloatBlock);
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}
			//	if (primitiveBoolBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(primitiveBoolBlock);
			//		if (returnBlock != nullptr)
			//		{
			//			builder->CreateBr(returnBlock);
			//		}
			//	}
			//	if (returnBlock != nullptr)
			//	{
			//		builder->SetInsertPoint(returnBlock);
			//	}
			//}
		}
		void EnsureCheckedMethodInstruction::Print(bool resolve)
		{
			cout << "EnsureCheckedMethod ";
			NomConstants::PrintConstant(MethodName, resolve);
			cout << "@" << Receiver;
			cout << "\n";
		}
		void EnsureCheckedMethodInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
			result.push_back(MethodName);
		}
	}
}