#include "UnaryOpInstruction.h"
#include <iostream>
#include <stdio.h>
#include "IntClass.h"
#include "FloatClass.h"
#include "BoolClass.h"
#include "NomClassType.h"
#include "NomValue.h"
#include "CompileHelpers.h"
#include "RefValueHeader.h"
#include "RTClass.h"
#include "NomDynamicType.h"
#include "RTOutput.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		const char* GetUnaryOpName(UnaryOperation op)
		{
			switch (op)
			{
			case UnaryOperation::Negate:
				return "-";
			case UnaryOperation::Not:
				return "!";
			}
		}
		UnaryOpInstruction::UnaryOpInstruction(const UnaryOperation op, const RegIndex arg, const RegIndex reg) : NomValueInstruction(reg, OpCode::UnaryOp), Operation(op), Arg(arg)
		{
		}
		UnaryOpInstruction::~UnaryOpInstruction()
		{
		}
		void UnaryOpInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			switch (this->Operation)
			{
			case UnaryOperation::Negate: {
				BasicBlock* currentBlock = builder->GetInsertBlock();
				Function* fun = currentBlock->getParent();
				BasicBlock* refObjectBlock = nullptr, * intBlock = nullptr, * floatBlock = nullptr, * boolBlock = nullptr;
				Value* intValue, * floatValue, * boolValue;

				NomValue val = ((*env)[Arg]);

				unsigned int cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, val, &refObjectBlock, nullptr, nullptr, true, &intBlock, &intValue, &floatBlock, &floatValue, &boolBlock, &boolValue);
				if (refObjectBlock!=nullptr)
				{
					cases = 0;
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to negate non-numeric value!", refObjectBlock);
				}
				if (boolBlock != nullptr)
				{
					cases = 0;
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Tried to numerically negate boolean value!", boolBlock);
				}

				BasicBlock* mergeBlock = nullptr;
				PHINode* mergePHI = nullptr;
				Value* mergeResult = nullptr;
				NomTypeRef singleCaseType = nullptr;
				if (cases > 1)
				{
					mergeBlock = BasicBlock::Create(LLVMCONTEXT, "negateMerge", fun);
					builder->SetInsertPoint(mergeBlock);
					mergePHI=builder->CreatePHI(REFTYPE, 2, "negateOut");
					mergeResult = mergePHI;
				}

				if (intBlock != nullptr)
				{
					builder->SetInsertPoint(intBlock);
					auto negatedInt = builder->CreateNeg(intValue, "negatedInt");
					if (mergePHI != nullptr)
					{
						mergePHI->addIncoming(negatedInt, builder->GetInsertBlock());
						builder->CreateBr(mergeBlock);
					}
					else
					{
						singleCaseType = NomIntClass::GetInstance()->GetType();
						mergeResult = negatedInt;
					}
				}

				if (floatBlock != nullptr)
				{
					builder->SetInsertPoint(intBlock);
					auto negatedFloat = builder->CreateNeg(intValue, "negatedFloat");
					if (mergePHI != nullptr)
					{
						mergePHI->addIncoming(negatedFloat, builder->GetInsertBlock());
						builder->CreateBr(mergeBlock);
					}
					else
					{
						singleCaseType = NomFloatClass::GetInstance()->GetType();
						mergeResult = negatedFloat;
					}
				}

				if (mergePHI)
				{
					builder->SetInsertPoint(mergeBlock);
					RegisterValue(env, NomValue(mergeResult, &NomDynamicType::Instance(), false));
				}
				else
				{
					RegisterValue(env, NomValue(mergeResult, singleCaseType, false));
				}

				//	if ((*env)[Arg]->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()) || (*env)[Arg].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
				//	{
				//		RegisterValue(env, NomValue(builder->CreateNeg(EnsureUnpackedInt(builder, env, (*env)[Arg])), NomIntClass::GetInstance()->GetType(), false));
				//		return;
				//	}
				//if ((*env)[Arg]->getType()->isDoubleTy() || (*env)[Arg].GetNomType()->IsSubtype(NomFloatClass::GetInstance()->GetType()))
				//{
				//	RegisterValue(env, NomValue(builder->CreateFNeg(EnsureUnpackedFloat(builder, env, (*env)[Arg])), NomFloatClass::GetInstance()->GetType(), false));
				//	return;
				//}
				//Value* val = (*env)[Arg];

				//BasicBlock* currentBlock = builder->GetInsertBlock();
				//BasicBlock* intBlock = BasicBlock::Create(LLVMCONTEXT, "negateInt", currentBlock->getParent());
				//BasicBlock* floatBlock = BasicBlock::Create(LLVMCONTEXT, "negateFloat", currentBlock->getParent());
				//BasicBlock* errorBlock = BasicBlock::Create(LLVMCONTEXT, "negateError", currentBlock->getParent());
				//BasicBlock* refBlock = BasicBlock::Create(LLVMCONTEXT, "negateRef", currentBlock->getParent());
				//BasicBlock* tryFloatBlock = BasicBlock::Create(LLVMCONTEXT, "tryFloat", currentBlock->getParent());
				//BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "negateOut", currentBlock->getParent());

				//CreateRefKindSwitch(builder, val, refBlock, refBlock, intBlock, floatBlock);

				//builder->SetInsertPoint(refBlock);
				//auto vtableptr = builder->CreatePtrToInt(RefValueHeader::GenerateReadVTablePointer(builder, val), INTTYPE);
				//auto inttableptr = ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*currentBlock->getParent()->getParent()), INTTYPE);
				//auto vtablematch = builder->CreateICmpEQ(vtableptr, inttableptr);
				//builder->CreateCondBr(vtablematch, intBlock, tryFloatBlock);

				//builder->SetInsertPoint(tryFloatBlock);
				//auto floattableptr = ConstantExpr::getPtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*currentBlock->getParent()->getParent()), INTTYPE);
				//vtablematch = builder->CreateICmpEQ(vtableptr, floattableptr);
				//builder->CreateCondBr(vtablematch, floatBlock, errorBlock);

				//builder->SetInsertPoint(intBlock);
				//auto unpackedInt = UnpackInt(builder, val, false);
				//auto negatedInt = builder->CreateNeg(unpackedInt, "negatedInt");
				//auto packedNegatedInt = PackInt(builder, negatedInt);
				//builder->CreateBr(outBlock);

				//builder->SetInsertPoint(floatBlock);
				//auto unpackedFloat = UnpackFloat(builder, val, false);
				//auto negatedFloat = builder->CreateFNeg(unpackedFloat, "negatedFloat");
				//auto packedNegatedFloat = PackFloat(builder, negatedFloat);
				//builder->CreateBr(outBlock);

				//builder->SetInsertPoint(errorBlock);
				//static const char* negate_errorMessage = "Tried to negate non-numeric value!";
				//builder->CreateCall(RTOutput_Fail::GetLLVMElement(*currentBlock->getParent()->getParent()), GetLLVMPointer(negate_errorMessage))->setCallingConv(RTOutput_Fail::GetLLVMElement(*currentBlock->getParent()->getParent())->getCallingConv());
				//CreateDummyReturn(builder, currentBlock->getParent());


				//builder->SetInsertPoint(outBlock);
				//auto outPhi = builder->CreatePHI(REFTYPE, 2, "negatedValue");
				//outPhi->addIncoming(packedNegatedInt, intBlock);
				//outPhi->addIncoming(packedNegatedFloat, floatBlock);
				//RegisterValue(env, NomValue(outPhi, &NomDynamicType::Instance(), false));
				//return;
				break;
			}
			case UnaryOperation::Not: {
				RegisterValue(env, NomValue(PackBool(builder, builder->CreateNot(EnsureUnpackedBool(builder, env, (*env)[Arg]))), NomBoolClass::GetInstance()->GetType(), false));
				return;
			}
			}
		}
		void UnaryOpInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "UnaryOp";
			cout << " " << GetUnaryOpName(Operation) << " #";
			cout << std::dec << Arg;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}
		void UnaryOpInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
