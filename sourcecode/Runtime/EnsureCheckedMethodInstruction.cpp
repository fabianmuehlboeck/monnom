#include "EnsureCheckedMethodInstruction.h"
#include <stdio.h>
#include "NomConstants.h"
#include "RefValueHeader.h"
#include "CompileEnv.h"
#include "NomClassType.h"
#include "RTOutput.h"
#include "RTStruct.h"
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
			auto methodName = NomConstants::GetString(MethodName)->GetText()->ToStdString();
			if (((!RTConfig_IgnoreEnsureMethod)&&!methodName.empty())|| RTConfig_AlwaysEnsureMethod)
			{
				BasicBlock* refValueBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
				auto receiver = (*env)[Receiver];
				int cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, receiver, &refValueBlock, &intBlock, &floatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);
				BasicBlock* returnBlock = nullptr;
				if (cases > 1)
				{
					returnBlock = BasicBlock::Create(LLVMCONTEXT, "ensureMethodOut", builder->GetInsertBlock()->getParent());
				}

				if (refValueBlock != nullptr)
				{
					builder->SetInsertPoint(refValueBlock);

					if (receiver.GetNomType()->GetKind() == TypeKind::TKClass && !((NomClassTypeRef)receiver.GetNomType())->Named->IsInterface())
					{
						//nothing to do here
						if (returnBlock != nullptr)
						{
							builder->CreateBr(returnBlock);
						}
					}
					else
					{
						BasicBlock* classBlock = nullptr, * lambdaBlock = nullptr, * structBlock = nullptr, * partialAppBlock = nullptr;
						Value* vTableVar = nullptr, * sTableVar = nullptr;
						if (methodName.length() != 0)
						{
							std::string* invalidLambdaMethodMsg = new std::string("Invalid method call on Lambda: " + methodName);
							lambdaBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, invalidLambdaMethodMsg->c_str());
						}
						RefValueHeader::GenerateVTableTagSwitch(builder, receiver, &vTableVar, &sTableVar, &classBlock, &lambdaBlock, &structBlock, &partialAppBlock);
						if (returnBlock == nullptr)
						{
							returnBlock = BasicBlock::Create(LLVMCONTEXT, "ensureMethodOut", builder->GetInsertBlock()->getParent());
						}
						if (classBlock != nullptr)
						{
							builder->SetInsertPoint(classBlock);
							if (returnBlock != nullptr)
							{
								builder->CreateBr(returnBlock);
							}
						}
						if (lambdaBlock != nullptr && methodName.length() == 0)
						{
							builder->SetInsertPoint(lambdaBlock);
							if (returnBlock != nullptr)
							{
								builder->CreateBr(returnBlock);
							}
						}
						if (structBlock != nullptr)
						{
							if (returnBlock == nullptr)
							{
								returnBlock = BasicBlock::Create(LLVMCONTEXT, "ensureMethodOut", builder->GetInsertBlock()->getParent());;
							}
							std::string* invalidStructMethodMsg = new std::string("No such method exists in given struct: " + methodName);
							BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, invalidStructMethodMsg->c_str());
							builder->SetInsertPoint(structBlock);
							auto methodEnsureFun = RTStruct::GenerateReadMethodEnsure(builder, sTableVar);
							auto methodEnsureCall = builder->CreateCall(GetMethodEnsureFunctionType(), methodEnsureFun, { receiver, MakeInt<size_t>(NomNameRepository::Instance().GetNameID(methodName)) });
							methodEnsureCall->setCallingConv(NOMCC);
							builder->CreateIntrinsic(Intrinsic::expect, { inttype(1) }, { methodEnsureCall, MakeUInt(1,1) });
							builder->CreateCondBr(methodEnsureCall, returnBlock, errorBlock);
						}
						if (partialAppBlock != nullptr)
						{
							builder->SetInsertPoint(partialAppBlock);
							if (methodName.length() == 0)
							{
								if (returnBlock != nullptr)
								{
									builder->CreateBr(returnBlock);
								}
							}
							else
							{
								std::string* invalidPartialAppMethodMsg = new std::string("Invalid method call on Partial Application: " + methodName);
								RTOutput_Fail::MakeBlockFailOutputBlock(builder, invalidPartialAppMethodMsg->c_str(), partialAppBlock);
							}
						}
					}
				}

				if (intBlock != nullptr)
				{
					builder->SetInsertPoint(intBlock);
					if (returnBlock != nullptr)
					{
						builder->CreateBr(returnBlock);
					}
				}
				if (floatBlock != nullptr)
				{
					builder->SetInsertPoint(floatBlock);
					if (returnBlock != nullptr)
					{
						builder->CreateBr(returnBlock);
					}
				}
				if (primitiveIntBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveIntBlock);
					if (returnBlock != nullptr)
					{
						builder->CreateBr(returnBlock);
					}
				}
				if (primitiveFloatBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveFloatBlock);
					if (returnBlock != nullptr)
					{
						builder->CreateBr(returnBlock);
					}
				}
				if (primitiveBoolBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveBoolBlock);
					if (returnBlock != nullptr)
					{
						builder->CreateBr(returnBlock);
					}
				}
				if (returnBlock != nullptr)
				{
					builder->SetInsertPoint(returnBlock);
				}
			}
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