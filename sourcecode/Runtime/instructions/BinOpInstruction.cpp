#include "BinOpInstruction.h"
#include "llvm/IR/BasicBlock.h"
#include "../Context.h"
#include "../FloatClass.h"
#include "../IntClass.h"
#include "../BoolClass.h"
#include "../NomClassType.h"
#include "../TypeOperations.h"
#include "../CompileHelpers.h"
#include <iostream>
#include "../TypeOperations.h"
#include "CastInstruction.h"
#include "../NomValuePrimitiveCases.h"
#include "../RefValueHeader.h"
#include "../RTOutput.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		BinOpInstruction::BinOpInstruction(const BinaryOperation op, const RegIndex left, const RegIndex right, const RegIndex reg) : NomValueInstruction(reg, OpCode::BinOp), Operation(op), Left(left), Right(right)
		{
		}


		BinOpInstruction::~BinOpInstruction()
		{
		}

		void BinOpInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			static auto boolType = NomBoolClass::GetInstance()->GetType();
			/*if ((*env)[Left]->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()) || (*env)[Left].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			{
				RegisterValue(env, CompileLeftInt(builder, env, lineno, EnsureUnpackedInt(builder, env, (*env)[Left])));
			}
			else if ((*env)[Left]->getType()->isDoubleTy() || (*env)[Left].GetNomType()->IsSubtype(NomFloatClass::GetInstance()->GetType()))
			{
				RegisterValue(env, CompileLeftFloat(builder, env, lineno, EnsureUnpackedFloat(builder, env, (*env)[Left])));
			}
			else if ((*env)[Left]->getType()->isIntegerTy(1) || (*env)[Left].GetNomType()->IsSubtype(boolType))
			{
				RegisterValue(env, CompileLeftBool(builder, env, lineno, EnsureUnpackedBool(builder, env, (*env)[Left])));
			}
			else*/ if (this->Operation == BinaryOperation::RefEquals)
			{
				auto inttype = NomIntClass::GetInstance()->GetType();
				auto floattype = NomFloatClass::GetInstance()->GetType();
				auto left = (*env)[Left];
				auto right = (*env)[Right];

				auto baseeq = builder->CreateICmpEQ(builder->CreatePtrToInt(left, INTTYPE, "leftAddr"), builder->CreatePtrToInt(right, INTTYPE, "rightaddr"), "refequal");

				auto lefttype = left.GetNomType();
				auto righttype = right.GetNomType();

				if (lefttype->IsDisjoint(righttype) || (lefttype->IsDisjoint(inttype) && lefttype->IsDisjoint(floattype)) || (righttype->IsDisjoint(inttype) && righttype->IsDisjoint(floattype)))
				{
					RegisterValue(env, NomValue(baseeq, NomBoolClass::GetInstance()->GetType()));
					return;
				}

				llvm::BasicBlock* startBlock = builder->GetInsertBlock();
				llvm::BasicBlock* neqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NEQ", env->Function);
				llvm::BasicBlock* outblock = llvm::BasicBlock::Create(LLVMCONTEXT, "REFEQOUT", env->Function);

				builder->CreateCondBr(baseeq, outblock, neqblock);

				builder->SetInsertPoint(neqblock);
				NomValue ret;
				GeneratePrimitiveTypeCase(builder, left,
					[&](NomBuilder& builder, NomValue& lval, NomValue* larr)
					{
						GeneratePrimitiveTypeCase(builder, right,
							[&](NomBuilder& builder, NomValue& rval, NomValue* rarr)
							{
								rarr[0] = NomValue(builder->CreateICmpEQ(lval, rval, "inteq"), boolType, false);
							},
							[&](NomBuilder& builder, NomValue& rval, NomValue* rarr)
							{
								rarr[0] = NomValue(ConstantInt::get(inttype(1), 0), boolType, false);
							},
								[&](NomBuilder& builder, NomValue& rval, llvm::Value* rvtblptr, NomValue* rarr)
							{
								rarr[0] = NomValue(ConstantInt::get(inttype(1), 0), boolType, false);
							},
								larr, 1);
					},
					[&](NomBuilder& builder, NomValue& lval, NomValue* larr)
					{
						NomValue iret;
						GeneratePrimitiveTypeCase(builder, right,
							[&](NomBuilder& builder, NomValue& rval, NomValue* rarr)
							{
								rarr[0] = NomValue(ConstantInt::get(inttype(1), 0), boolType, false);
							},
							[&](NomBuilder& builder, NomValue& rval, NomValue* rarr)
							{
								rarr[0] = NomValue(builder->CreateFCmpOEQ(lval, rval, "floateq"), boolType, false);
							},
								[&](NomBuilder& builder, NomValue& rval, llvm::Value* rvtblptr, NomValue* rarr)
							{
								rarr[0] = NomValue(ConstantInt::get(inttype(1), 0), boolType, false);
							},
								larr, 1);
					},
						[&](NomBuilder& builder, NomValue& lval, llvm::Value* lvtblptr, NomValue* larr)
					{
						larr[0] = NomValue(ConstantInt::get(inttype(1), 0), boolType, false);
					},
						&ret, 1);
				neqblock = builder->GetInsertBlock();
				builder->CreateBr(outblock);

				builder->SetInsertPoint(outblock);
				auto outPHI = builder->CreatePHI(inttype(1), 2);
				outPHI->addIncoming(baseeq, startBlock);
				outPHI->addIncoming(ret, neqblock);

				RegisterValue(env, NomValue(outPHI, NomBoolClass::GetInstance()->GetType()));
			}
			else
			{
				auto leftVal = (*env)[Left];

				BasicBlock* refValueBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
				Value* leftIntValue = nullptr, * leftFloatValue = nullptr, * leftBoolValue = nullptr;
				Function* fun = builder->GetInsertBlock()->getParent();

				auto cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, leftVal, &refValueBlock, nullptr, nullptr, true, &primitiveIntBlock, &leftIntValue, &primitiveFloatBlock, &leftFloatValue, &primitiveBoolBlock, &leftBoolValue);

				if (refValueBlock != nullptr)
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Invalid binary operation on non-primitive operand!", refValueBlock);
					cases--;
				}

				if (cases == 0)
				{
					throw new std::exception();
				}

				BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "binOpOut", fun);
				PHINode* outPHI = nullptr;;
				NomValue outValue;
				if (cases > 1)
				{
					builder->SetInsertPoint(outBlock);
					outPHI = builder->CreatePHI(REFTYPE, cases, "binOpResult");
					outValue = NomValue(outPHI);
				}
				if (primitiveIntBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveIntBlock);
					auto retVal = CompileLeftInt(builder, env, lineno, leftIntValue);
					if (retVal->getValueID()== Value::PoisonValueVal)
					{
						if (builder->GetInsertBlock()->getTerminator() == nullptr)
						{
							BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
							builder->CreateBr(invalidBlock);
						}
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
					}
					else
					{
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
						else
						{
							retVal = EnsurePacked(builder, retVal);
							outPHI->addIncoming(retVal, builder->GetInsertBlock());
						}
						builder->CreateBr(outBlock);
					}
				}
				if (primitiveFloatBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveFloatBlock);
					auto retVal = CompileLeftFloat(builder, env, lineno, leftFloatValue);
					if (retVal->getValueID() == Value::PoisonValueVal)
					{
						if (builder->GetInsertBlock()->getTerminator() == nullptr)
						{
							BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
							builder->CreateBr(invalidBlock);
						}
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
					}
					else
					{
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
						else
						{
							retVal = EnsurePacked(builder, retVal);
							outPHI->addIncoming(retVal, builder->GetInsertBlock());
						}
						builder->CreateBr(outBlock);
					}
				}
				if (primitiveBoolBlock != nullptr)
				{
					builder->SetInsertPoint(primitiveBoolBlock);
					auto retVal = CompileLeftBool(builder, env, lineno, leftBoolValue);
					if (retVal->getValueID() == Value::PoisonValueVal)
					{
						if (builder->GetInsertBlock()->getTerminator() == nullptr)
						{
							BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
							builder->CreateBr(invalidBlock);
						}
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
					}
					else
					{
						if (outPHI == nullptr)
						{
							outValue = retVal;
						}
						else
						{
							retVal = EnsurePacked(builder, retVal);
							outPHI->addIncoming(retVal, builder->GetInsertBlock());
						}
						builder->CreateBr(outBlock);
					}
				}

				builder->SetInsertPoint(outBlock);
				RegisterValue(env, outValue);

				//llvm::BasicBlock* intblock = llvm::BasicBlock::Create(LLVMCONTEXT, "INT_L", env->Function);
				//llvm::BasicBlock* elseblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NOTINT_L", env->Function);
				//llvm::BasicBlock* floatblock = llvm::BasicBlock::Create(LLVMCONTEXT, "FLOAT_L", env->Function);
				//llvm::BasicBlock* clsblock = llvm::BasicBlock::Create(LLVMCONTEXT, "REF_L", env->Function);
				//llvm::BasicBlock* outblock = llvm::BasicBlock::Create(LLVMCONTEXT, "OUT_L", env->Function);

				////Check Int
				//llvm::Value* isint = IsInt(builder, (*env)[Left]);
				///*llvm::BranchInst *intbranch = */builder->CreateCondBr(isint, intblock, elseblock, (llvm::Instruction*)nullptr);

				////If Int
				//builder->SetInsertPoint(intblock);
				//llvm::Value* leftIntResult = EnsurePacked(builder, CompileLeftInt(builder, env, lineno, UnpackInt(builder, (*env)[Left])));
				//intblock = builder->GetInsertBlock();
				//builder->CreateBr(outblock);

				////Else
				//builder->SetInsertPoint(elseblock);
				////Check Float
				//llvm::Value* isfloat = IsFloat(builder, (*env)[Left]);
				///*llvm::BranchInst *floatbranch =*/ builder->CreateCondBr(isfloat, floatblock, clsblock, (llvm::Instruction*)nullptr);

				////If Float
				//builder->SetInsertPoint(floatblock);
				//llvm::Value* leftFloatResult = EnsurePacked(builder, CompileLeftFloat(builder, env, lineno, UnpackFloat(builder, (*env)[Left])));
				//floatblock = builder->GetInsertBlock();
				//builder->CreateBr(outblock);

				////If Pointer
				//builder->SetInsertPoint(clsblock);
				//llvm::Value* rightval = (*env)[Right];
				//if (rightval->getType()->isIntegerTy())
				//{
				//	rightval = PackInt(builder, rightval);
				//}
				//else if (rightval->getType()->isDoubleTy())
				//{
				//	rightval = PackFloat(builder, rightval);
				//}
				//llvm::Value* leftClsResult = CompileLeftPointer(builder, env, lineno, rightval);
				//clsblock = builder->GetInsertBlock();
				//builder->CreateBr(outblock);

				////Phi and return
				//builder->SetInsertPoint(outblock);
				//llvm::PHINode* phi = builder->CreatePHI(REFTYPE, 3);
				//phi->addIncoming(leftIntResult, intblock);
				//phi->addIncoming(leftFloatResult, floatblock);
				//phi->addIncoming(leftClsResult, clsblock);
				//RegisterValue(env, NomValue(phi, NomType::Anything));
			}
		}

		void BinOpInstruction::Print(bool resolve)
		{
			cout << "BinOp #";
			cout << std::dec << Left;
			cout << " " << GetBinOpName(Operation) << " #";
			cout << std::dec << Right;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}

		void BinOpInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}

		NomValue BinOpInstruction::CompileLeftInt(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left)
		{
			auto rightVal = (*env)[Right];

			BasicBlock* refValueBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			Value* rightIntValue = nullptr, * rightFloatValue = nullptr, * rightBoolValue = nullptr;
			Function* fun = builder->GetInsertBlock()->getParent();

			auto cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, rightVal, &refValueBlock, nullptr, nullptr, true, &primitiveIntBlock, &rightIntValue, &primitiveFloatBlock, &rightFloatValue, &primitiveBoolBlock, &rightBoolValue);

			if (refValueBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Invalid binary operation on non-primitive operand!", refValueBlock);
				cases--;
			}
			if (primitiveBoolBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "No binary operations on Integers and Booleans!", primitiveBoolBlock);
				cases--;
			}
			if (cases == 0)
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}
			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "binOpOut$leftInt", fun);
			PHINode* outPHI = nullptr;
			NomValue outValue;
			if (cases > 1)
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(REFTYPE, cases, "binOpResult$leftInt");
				outValue = NomValue(outPHI);
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				auto retVal = CompileIntInt(builder, env, lineno, left, rightIntValue);
				if (retVal->getValueID() == Value::PoisonValueVal)
				{

					if (builder->GetInsertBlock()->getTerminator() == nullptr)
					{
						BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
						builder->CreateBr(invalidBlock);
					}
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
				}
				else
				{
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
					else
					{
						retVal = EnsurePacked(builder, retVal);
						outPHI->addIncoming(retVal, builder->GetInsertBlock());
					}
					builder->CreateBr(outBlock);
				}
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				auto retVal = CompileFloatFloat(builder, env, lineno, builder->CreateSIToFP(left, FLOATTYPE, "leftAsFloat"), rightFloatValue);
				if (retVal->getValueID() == Value::PoisonValueVal)
				{

					if (builder->GetInsertBlock()->getTerminator() == nullptr)
					{
						BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
						builder->CreateBr(invalidBlock);
					}
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
				}
				else
				{
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
					else
					{
						retVal = EnsurePacked(builder, retVal);
						outPHI->addIncoming(retVal, builder->GetInsertBlock());
					}
					builder->CreateBr(outBlock);
				}
			}
			builder->SetInsertPoint(outBlock);
			return outValue;

			//if ((*env)[Right]->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()) || (*env)[Right].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	return CompileIntInt(builder, env, lineno, left, EnsureUnpackedInt(builder, env, (*env)[Right]));
			//}
			//else if ((*env)[Right]->getType()->isDoubleTy() || (*env)[Right].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	return CompileFloatFloat(builder, env, lineno, builder->CreateSIToFP(left, FLOATTYPE), EnsureUnpackedFloat(builder, env, (*env)[Right]));
			//}
			//else if ((*env)[Right]->getType()->isIntegerTy(1))
			//{
			//	throw new std::exception();
			//}
			//else
			//{
			//	llvm::BasicBlock* intblock = llvm::BasicBlock::Create(LLVMCONTEXT, "INT_LIR", env->Function);
			//	llvm::BasicBlock* elseblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NOTINT_LIR", env->Function);
			//	llvm::BasicBlock* floatblock = llvm::BasicBlock::Create(LLVMCONTEXT, "FLOAT_LIR", env->Function);
			//	llvm::BasicBlock* clsblock = llvm::BasicBlock::Create(LLVMCONTEXT, "REF_LIR", env->Function);
			//	llvm::BasicBlock* outblock = llvm::BasicBlock::Create(LLVMCONTEXT, "OUT_LIR", env->Function);

			//	//Check Int
			//	llvm::Value* isint = IsInt(builder, (*env)[Right]);
			//	/*llvm::BranchInst *intbranch = */builder->CreateCondBr(isint, intblock, elseblock, (llvm::Instruction*)nullptr);

			//	//If Int
			//	builder->SetInsertPoint(intblock);
			//	llvm::Value* intResult = EnsurePacked(builder, CompileIntInt(builder, env, lineno, left, UnpackInt(builder, (*env)[Right])));
			//	intblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//Else
			//	builder->SetInsertPoint(elseblock);
			//	//Check Float
			//	llvm::Value* isfloat = IsFloat(builder, (*env)[Right]);
			//	/*llvm::BranchInst *floatbranch =*/ builder->CreateCondBr(isfloat, floatblock, clsblock, (llvm::Instruction*)nullptr);

			//	//If Float
			//	builder->SetInsertPoint(floatblock);
			//	llvm::Value* floatResult = EnsurePacked(builder, CompileFloatFloat(builder, env, lineno, builder->CreateSIToFP(left, FLOATTYPE), UnpackFloat(builder, (*env)[Right])));
			//	floatblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//If Pointer
			//	builder->SetInsertPoint(clsblock);
			//	auto packedInt = PackInt(builder, left);
			//	llvm::Value* clsResult = CompileLeftPointer(builder, env, lineno, packedInt);
			//	clsblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//Phi and return
			//	builder->SetInsertPoint(outblock);
			//	llvm::PHINode* phi = builder->CreatePHI(REFTYPE, 3);
			//	phi->addIncoming(intResult, intblock);
			//	phi->addIncoming(floatResult, floatblock);
			//	phi->addIncoming(clsResult, clsblock);
			//	return NomValue(phi, NomType::Anything);
			//}
		}
		NomValue BinOpInstruction::CompileLeftFloat(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left)
		{
			auto rightVal = (*env)[Right];

			BasicBlock* refValueBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			Value* rightIntValue = nullptr, * rightFloatValue = nullptr, * rightBoolValue = nullptr;
			Function* fun = builder->GetInsertBlock()->getParent();

			auto cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, rightVal, &refValueBlock, nullptr, nullptr, true, &primitiveIntBlock, &rightIntValue, &primitiveFloatBlock, &rightFloatValue, &primitiveBoolBlock, &rightBoolValue);

			if (refValueBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Invalid binary operation on non-primitive operand!", refValueBlock);
				cases--;
			}
			if (primitiveBoolBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "No binary operations on Floats and Booleans!", primitiveBoolBlock);
				cases--;
			}
			if (cases == 0)
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "binOpOut$leftFloat", fun);
			PHINode* outPHI = nullptr;
			NomValue outValue;
			if (cases > 1)
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(REFTYPE, cases, "binOpResult$leftFloat");
				outValue = NomValue(outPHI);
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				auto retVal = CompileFloatFloat(builder, env, lineno, left, builder->CreateSIToFP(rightIntValue, FLOATTYPE, "rightAsFloat"));
				if (retVal->getValueID() == Value::PoisonValueVal)
				{

					if (builder->GetInsertBlock()->getTerminator() == nullptr)
					{
						BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
						builder->CreateBr(invalidBlock);
					}
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
				}
				else
				{
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
					else
					{
						retVal = EnsurePacked(builder, retVal);
						outPHI->addIncoming(retVal, builder->GetInsertBlock());
					}
					builder->CreateBr(outBlock);
				}
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				auto retVal = CompileFloatFloat(builder, env, lineno, left, rightFloatValue); if (retVal->getValueID() == Value::PoisonValueVal)
				{
					if (builder->GetInsertBlock()->getTerminator() == nullptr)
					{
						BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
						builder->CreateBr(invalidBlock);
					}
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
				}
				else
				{
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
					else
					{
						retVal = EnsurePacked(builder, retVal);
						outPHI->addIncoming(retVal, builder->GetInsertBlock());
					}
					builder->CreateBr(outBlock);
				}
			}
			builder->SetInsertPoint(outBlock);
			return outValue;

			//if ((*env)[Right]->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()) || (*env)[Right].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	return CompileFloatFloat(builder, env, lineno, left, builder->CreateSIToFP(EnsureUnpackedInt(builder, env, (*env)[Right]), FLOATTYPE));
			//}
			//else if ((*env)[Right]->getType()->isDoubleTy() || (*env)[Left].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	return CompileFloatFloat(builder, env, lineno, left, EnsureUnpackedFloat(builder, env, (*env)[Right]));
			//}
			//else if ((*env)[Right]->getType()->isIntegerTy(1))
			//{
			//	throw new std::exception();
			//}
			//else
			//{
			//	llvm::BasicBlock* intblock = llvm::BasicBlock::Create(LLVMCONTEXT, "INT_LFR", env->Function);
			//	llvm::BasicBlock* elseblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NOTINT_LFR", env->Function);
			//	llvm::BasicBlock* floatblock = llvm::BasicBlock::Create(LLVMCONTEXT, "FLOAT_LFR", env->Function);
			//	llvm::BasicBlock* clsblock = llvm::BasicBlock::Create(LLVMCONTEXT, "REF_LFR", env->Function);
			//	llvm::BasicBlock* outblock = llvm::BasicBlock::Create(LLVMCONTEXT, "OUT_LFR", env->Function);

			//	//Check Int
			//	llvm::Value* isint = IsInt(builder, (*env)[Right]);
			//	/*llvm::BranchInst *intbranch = */builder->CreateCondBr(isint, intblock, elseblock, (llvm::Instruction*)nullptr);

			//	//If Int
			//	builder->SetInsertPoint(intblock);
			//	llvm::Value* intResult = EnsurePacked(builder, CompileFloatFloat(builder, env, lineno, left, builder->CreateSIToFP(UnpackInt(builder, (*env)[Right]), FLOATTYPE)));
			//	intblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//Else
			//	builder->SetInsertPoint(elseblock);
			//	//Check Float
			//	llvm::Value* isfloat = IsFloat(builder, (*env)[Right]);
			//	/*llvm::BranchInst *floatbranch =*/ builder->CreateCondBr(isfloat, floatblock, clsblock, (llvm::Instruction*)nullptr);

			//	//If Float
			//	builder->SetInsertPoint(floatblock);
			//	llvm::Value* floatResult = EnsurePacked(builder, CompileFloatFloat(builder, env, lineno, left, UnpackFloat(builder, (*env)[Right])));
			//	floatblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//If Pointer
			//	builder->SetInsertPoint(clsblock);
			//	llvm::Value* clsResult = CompileLeftPointer(builder, env, lineno, PackFloat(builder, left));
			//	clsblock = builder->GetInsertBlock();
			//	builder->CreateBr(outblock);

			//	//Phi and return
			//	builder->SetInsertPoint(outblock);
			//	llvm::PHINode* phi = builder->CreatePHI(REFTYPE, 3);
			//	phi->addIncoming(intResult, intblock);
			//	phi->addIncoming(floatResult, floatblock);
			//	phi->addIncoming(clsResult, clsblock);
			//	return NomValue(phi, NomType::Anything);
		}
		NomValue BinOpInstruction::CompileLeftBool(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left)
		{
			auto rightVal = (*env)[Right];

			BasicBlock* refValueBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;
			Value* rightIntValue = nullptr, * rightFloatValue = nullptr, * rightBoolValue = nullptr;
			Function* fun = builder->GetInsertBlock()->getParent();

			auto cases = RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, rightVal, &refValueBlock, nullptr, nullptr, true, &primitiveIntBlock, &rightIntValue, &primitiveFloatBlock, &rightFloatValue, &primitiveBoolBlock, &rightBoolValue);

			if (refValueBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Invalid binary operation on non-primitive operand!", refValueBlock);
				cases--;
			}
			if (primitiveIntBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "No binary operations on Booleans and Integers!", primitiveIntBlock);
				cases--;
			}
			if (primitiveFloatBlock != nullptr)
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "No binary operations on Booleans and Floats!", primitiveFloatBlock);
				cases--;
			}

			if (cases == 0)
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}

			BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "binOpOut$leftBool", fun);
			PHINode* outPHI = nullptr;
			NomValue outValue;
			if (cases > 1)
			{
				builder->SetInsertPoint(outBlock);
				outPHI = builder->CreatePHI(REFTYPE, cases, "binOpResult$leftBool");
				outValue = NomValue(outPHI);
			}
			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				auto retVal = CompileBoolBool(builder, env, lineno, left, rightBoolValue);
				if (retVal->getValueID() == Value::PoisonValueVal)
				{

					if (builder->GetInsertBlock()->getTerminator() == nullptr)
					{
						BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
						builder->CreateBr(invalidBlock);
					}
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
				}
				else
				{
					if (outPHI == nullptr)
					{
						outValue = retVal;
					}
					else
					{
						retVal = EnsurePacked(builder, retVal);
						outPHI->addIncoming(retVal, builder->GetInsertBlock());
					}
					builder->CreateBr(outBlock);
				}
			}
			builder->SetInsertPoint(outBlock);
			return outValue;
			//if ((*env)[Right]->getType()->isIntegerTy(1) || (*env)[Left].GetNomType()->IsSubtype(NomBoolClass::GetInstance()->GetType()))
			//{
			//	return CompileBoolBool(builder, env, lineno, left, EnsureUnpackedBool(builder, env, (*env)[Right]));
			//}
			//else if ((*env)[Right]->getType()->isIntegerTy(INTTYPE->getPrimitiveSizeInBits()) || (*env)[Right].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	throw new std::exception();
			//}
			//else if ((*env)[Right]->getType()->isDoubleTy() || (*env)[Left].GetNomType()->IsSubtype(NomIntClass::GetInstance()->GetType()))
			//{
			//	throw new std::exception();
			//}
			//else
			//{
			//	auto asBool = CastInstruction::MakeCast(builder, env, (*env)[Right], NomBoolClass::GetInstance()->GetType());
			//	return CompileBoolBool(builder, env, lineno, left, EnsureUnpackedBool(builder, env, asBool));
			//}
		}
		NomValue BinOpInstruction::CompileLeftPointer(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left)
		{
			return NomValue(GetLLVMRef(nullptr), NomType::Anything);
		}
		NomValue BinOpInstruction::CompileIntInt(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::Add:
				return NomValue(builder->CreateAdd(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Subtract:
				return NomValue(builder->CreateSub(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Multiply:
				return NomValue(builder->CreateMul(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Divide:
				return NomValue(builder->CreateSDiv(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Mod:
				return NomValue(builder->CreateSRem(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::GreaterThan:
				return NomValue(builder->CreateICmpSGT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessThan:
				return NomValue(builder->CreateICmpSLT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::GreaterOrEqualTo:
				return NomValue(builder->CreateICmpSGE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessOrEqualTo:
				return NomValue(builder->CreateICmpSLE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return NomValue(builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return NomValue(builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			default:
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}
			}
		}
		NomValue BinOpInstruction::CompileFloatFloat(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::Add:
				return NomValue(builder->CreateFAdd(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Subtract:
				return NomValue(builder->CreateFSub(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Multiply:
				return NomValue(builder->CreateFMul(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Divide:
				return NomValue(builder->CreateFDiv(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return NomValue(builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return NomValue(builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Mod:
				return NomValue(builder->CreateFRem(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::GreaterThan:
				return NomValue(builder->CreateFCmpOGT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessThan:
				return NomValue(builder->CreateFCmpOLT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::GreaterOrEqualTo:
				return NomValue(builder->CreateFCmpOGE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessOrEqualTo:
				return NomValue(builder->CreateFCmpOLE(left, right), NomBoolClass::GetInstance()->GetType());
			default:
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}
			}
		}
		NomValue BinOpInstruction::CompileBoolBool(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::And:
				return NomValue(EnsurePacked(builder, builder->CreateAnd(left, right)), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Or:
				return NomValue(EnsurePacked(builder, builder->CreateOr(left, right)), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::BitXOR:
				return NomValue(EnsurePacked(builder, builder->CreateXor(left, right)), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return NomValue(EnsurePacked(builder, builder->CreateICmpEQ(left, right)), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return NomValue(EnsurePacked(builder, builder->CreateICmpEQ(left, right)), NomBoolClass::GetInstance()->GetType());
			default:
			{
				//auto nullVal = GetLLVMRef(nullptr);
				//nullVal->setName("INVALID_VALUE");
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
			}
			}
		}
		//NomValue BinOpInstruction::CompileIntInt(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right)
		//{
		//	switch (Operation)
		//	{
		//	case BinaryOperation::Add:
		//		return NomValue(PackInt(builder, builder->CreateAdd(left, right)), NomIntClass::GetInstance()->GetType());
		//	case BinaryOperation::Subtract:
		//		return NomValue(PackInt(builder, builder->CreateSub(left, right)), NomIntClass::GetInstance()->GetType());
		//	case BinaryOperation::Multiply:
		//		return NomValue(PackInt(builder, builder->CreateMul(left, right)), NomIntClass::GetInstance()->GetType());
		//	case BinaryOperation::Divide:
		//		return NomValue(PackInt(builder, builder->CreateSDiv(left, right)), NomIntClass::GetInstance()->GetType());
		//	case BinaryOperation::Mod:
		//		return NomValue(PackInt(builder, builder->CreateSRem(left, right)), NomIntClass::GetInstance()->GetType());
		//	case BinaryOperation::GreaterThan:
		//		return NomValue(PackBool(builder, builder->CreateICmpSGT(left, right)), NomBoolClass::GetInstance()->GetType());
		//	case BinaryOperation::LessThan:
		//		return NomValue(PackBool(builder, builder->CreateICmpSLT(left, right)), NomBoolClass::GetInstance()->GetType());
		//	case BinaryOperation::GreaterOrEqualTo:
		//		return NomValue(PackBool(builder, builder->CreateICmpSGE(left, right)), NomBoolClass::GetInstance()->GetType());
		//	case BinaryOperation::LessOrEqualTo:
		//		return NomValue(PackBool(builder, builder->CreateICmpSLE(left, right)), NomBoolClass::GetInstance()->GetType());
		//	case BinaryOperation::Equals:
		//		return NomValue(PackBool(builder, builder->CreateICmpEQ(left, right)), NomBoolClass::GetInstance()->GetType());
		//	case BinaryOperation::RefEquals:
		//		return NomValue(PackBool(builder, builder->CreateICmpEQ(left, right)), NomBoolClass::GetInstance()->GetType());
		//	default:
		//		return NomValue(GetLLVMRef(nullptr), NomType::Anything);
		//	}
		//}
		//NomValue BinOpInstruction::CompileFloatFloat(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right)
		//{
		//	switch (Operation)
		//	{
		//	case BinaryOperation::Add:
		//		return NomValue(PackFloat(builder, builder->CreateFAdd(left, right)), NomFloatClass::GetInstance()->GetType());
		//	case BinaryOperation::Subtract:
		//		return NomValue(PackFloat(builder, builder->CreateFSub(left, right)), NomFloatClass::GetInstance()->GetType());
		//	case BinaryOperation::Multiply:
		//		return NomValue(PackFloat(builder, builder->CreateFMul(left, right)), NomFloatClass::GetInstance()->GetType());
		//	case BinaryOperation::Divide:
		//		return NomValue(PackFloat(builder, builder->CreateFDiv(left, right)), NomFloatClass::GetInstance()->GetType());
		//	default:
		//		return NomValue(GetLLVMRef(nullptr), NomType::Anything);
		//	}
		//}

		const char* GetBinOpName(BinaryOperation op)
		{
			switch (op)
			{
			case BinaryOperation::Equals:
				return "===";
			case BinaryOperation::RefEquals:
				return "==";
			case BinaryOperation::Add:
				return "+";
			case BinaryOperation::Subtract:
				return "-";
			case BinaryOperation::Multiply:
				return "*";
			case BinaryOperation::Divide:
				return "/";
			case BinaryOperation::Power:
				return "^";
			case BinaryOperation::Mod:
				return "%";
			case BinaryOperation::Concat:
				return "++";
			case BinaryOperation::And:
				return "&&";
			case BinaryOperation::Or:
				return "||";
			case BinaryOperation::BitAND:
				return "&";
			case BinaryOperation::BitOR:
				return "|";
			case BinaryOperation::BitXOR:
				return "^|";
			case BinaryOperation::ShiftLeft:
				return "<<";
			case BinaryOperation::ShiftRight:
				return ">>";
			case BinaryOperation::LessThan:
				return "<";
			case BinaryOperation::GreaterThan:
				return ">";
			case BinaryOperation::LessOrEqualTo:
				return "<=";
			case BinaryOperation::GreaterOrEqualTo:
				return ">=";
			}
		}
	}
}
