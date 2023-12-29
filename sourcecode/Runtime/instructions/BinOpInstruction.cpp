#include "BinOpInstruction.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/BasicBlock.h"
POPDIAGSUPPRESSION
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
#include "../Metadata.h"
#include "../PWRefValue.h"
#include "../PWPacked.h"
#include "../PWFloat.h"
#include "../PWInt.h"
#include "../PWObject.h"
#include "../RTRawInt.h"
#include "../RTRawBool.h"
#include "../RTRawFloat.h"
#include "../NomDynamicType.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wswitch-enum"
#elif defined(__GNU__)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#elif defined(_MSC_VER)

#endif

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		class BinaryOperationImplementation
		{
		public:
			const BinaryOperation Operation;
			const std::string OperationName;
			const std::string MethodName;
			virtual ~BinaryOperationImplementation() {}
		protected:
			enum class ValueType { Object, Int, Float, Bool, Any };
			BinaryOperationImplementation(BinaryOperation op, std::string& opName, std::string& methodName) : Operation(op), OperationName(opName), MethodName(methodName)
			{
			}
			virtual int Handles(ValueType left, ValueType right)
			{
				if (left == ValueType::Any)
				{
					return Handles(ValueType::Object, right) + Handles(ValueType::Int, right) + Handles(ValueType::Float, right) + Handles(ValueType::Bool, right);
				}
				if (right == ValueType::Any)
				{
					return Handles(left, ValueType::Object) + Handles(left, ValueType::Int) + Handles(left, ValueType::Float) + Handles(left, ValueType::Bool);
				}
				return 0;
			}
		};


		BinOpInstruction::BinOpInstruction(const BinaryOperation op, const RegIndex left, const RegIndex right, const RegIndex reg) : NomValueInstruction(reg, OpCode::BinOp), Operation(op), Left(left), Right(right)
		{
		}


		BinOpInstruction::~BinOpInstruction()
		{
		}

		void BinOpInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			static auto boolType = NomBoolClass::GetInstance()->GetType();
			if (this->Operation == BinaryOperation::RefEquals)
			{
				auto left = (*env)[Left];
				auto right = (*env)[Right];
				if (left->getType() != right->getType())
				{
					RegisterValue(env, RTValue::GetValue(builder, MakeUInt(1, 0), GetBoolClassType()));
					return;
				}
				if (left->getType()->isIntegerTy())
				{
					RegisterValue(env, RTValue::GetValue(builder, builder->CreateICmpEQ(left, right, "isRefEqual"), GetBoolClassType()));
					return;
				}
				if (left->getType()->isDoubleTy())
				{
					RegisterValue(env, RTValue::GetValue(builder, builder->CreateFCmpOEQ(left, right, "isRefEqual"), GetBoolClassType()));
					return;
				}
				RegisterValue(env, RTValue::GetValue(builder, builder->CreateICmpEQ(builder->CreatePtrToInt(left, numtype(intptr_t)), builder->CreatePtrToInt(right, numtype(intptr_t)), "isRefEqual"), GetBoolClassType()));
				return;
			}
			else if(this->Operation == BinaryOperation::Equals || this->Operation == BinaryOperation::RefEquals)
			{
				auto inttype = NomIntClass::GetInstance()->GetType();
				auto floattype = NomFloatClass::GetInstance()->GetType();
				auto left = (*env)[Left];
				auto right = (*env)[Right];

				auto lefttype = left.GetNomType();
				auto righttype = right.GetNomType();

				llvm::BasicBlock* neqblock = nullptr;
				llvm::BasicBlock* eqblock = nullptr;
				if (left->getType() == right->getType())
				{
					if (left->getType() == REFTYPE)
					{
						auto baseeq = builder->CreateICmpEQ(builder->CreatePtrToInt(left, INTTYPE, "leftAddr"), builder->CreatePtrToInt(right, INTTYPE, "rightaddr"), "refequal");
						if (this->Operation==BinaryOperation::RefEquals)
						{
							RegisterValue(env, RTValue::GetValue(builder, baseeq, NomBoolClass::GetInstance()->GetType()));
							return;
						}
						llvm::BasicBlock* nrefeqblock = llvm::BasicBlock::Create(builder->getContext(), "NREFEQ", env->Function);
						neqblock = llvm::BasicBlock::Create(builder->getContext(), "NEQ", env->Function);
						eqblock = llvm::BasicBlock::Create(builder->getContext(), "EQ", env->Function);
						builder->CreateCondBr(baseeq, eqblock, nrefeqblock);
						builder->SetInsertPoint(nrefeqblock);
					}
					else if (left->getType() == INTTYPE)
					{
						RegisterValue(env, RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else if (left->getType() == FLOATTYPE)
					{
						RegisterValue(env, RTValue::GetValue(builder, builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else if (left->getType() == BOOLTYPE)
					{
						RegisterValue(env, RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else
					{
						std::cout << "Internal error: invalid binary operand types";
						throw new std::exception();
					}
				}
				else if (this->Operation == BinaryOperation::RefEquals)
				{
					RegisterValue(env, RTValue::GetValue(builder, NomBoolObjects::GetFalse(*builder->GetInsertBlock()->getParent()->getParent()), NomBoolClass::GetInstance()->GetType()));
					return;
				}
				if (neqblock == nullptr)
				{
					neqblock = llvm::BasicBlock::Create(builder->getContext(), "NEQ", env->Function);
				}
				if (eqblock == nullptr)
				{
					eqblock = llvm::BasicBlock::Create(builder->getContext(), "EQ", env->Function);
				}
				BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operands for primitive operation!");

				left->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
					[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> lval) -> void {
						builder->CreateBr(errorBlock);
					},
					[right, eqblock, neqblock, errorBlock](NomBuilder& builder, RTPWValuePtr<PWInt64> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval,eqblock,neqblock](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								builder->CreateCondBr(builder->CreateICmpEQ(lval, rval, "isIntEq"), eqblock, neqblock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								builder->CreateBr(errorBlock);
							}, true);
					},
					[right, eqblock, neqblock, errorBlock](NomBuilder& builder, RTPWValuePtr<PWFloat> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
						[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval, eqblock, neqblock](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								builder->CreateCondBr(builder->CreateFCmpOEQ(lval, rval, "isIntEq"), eqblock, neqblock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								builder->CreateBr(errorBlock);
							}, true);
					},
					[right, eqblock, neqblock, errorBlock](NomBuilder& builder, RTPWValuePtr<PWBool> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
						[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval, eqblock, neqblock](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								builder->CreateCondBr(builder->CreateICmpEQ(lval, rval, "isBoolEq"), eqblock, neqblock);
							}, true);
					},
					true);
				BasicBlock* outblock = BasicBlock::Create(builder->getContext(), "eqOut", env->Function);
				builder->SetInsertPoint(eqblock);
				builder->CreateBr(outblock);

				builder->SetInsertPoint(neqblock);
				builder->CreateBr(outblock);

				builder->SetInsertPoint(outblock);
				auto outPHI = builder->CreatePHI(inttype(1), 2);
				outPHI->addIncoming(MakeUInt(1, 1), eqblock);
				outPHI->addIncoming(MakeUInt(1, 0), neqblock);

				RegisterValue(env, RTValue::GetValue(builder, outPHI, NomBoolClass::GetInstance()->GetType()));
			}
			else
			{
				BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operands for primitive operation!");
				auto inttype = NomIntClass::GetInstance()->GetType();
				auto floattype = NomFloatClass::GetInstance()->GetType();
				auto left = (*env)[Left];
				auto right = (*env)[Right];

				auto lefttype = left.GetNomType();
				auto righttype = right.GetNomType();

				RTValuePtr* retVals = makealloca(RTValuePtr, 16);
				llvm::BasicBlock** retBlocks = makealloca(llvm::BasicBlock*, 16);
				int retCount = 0;

				left->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
					[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> lval) -> void {
						builder->CreateBr(errorBlock);
					},
					[right, retVals, retBlocks, &retCount, errorBlock, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWInt64> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
						[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval, retVals, retBlocks, &retCount, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								retVals[retCount] = CompileIntInt(builder, env, lineno, lval, rval);
								retBlocks[retCount] = builder->GetInsertBlock();
								retCount++;
							},
							[lval, retVals, retBlocks, &retCount, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								retVals[retCount] = CompileFloatFloat(builder, env, lineno, builder->CreateSIToFP(lval, FLOATTYPE, "leftToFloat"), rval);
								retBlocks[retCount] = builder->GetInsertBlock();
								retCount++;
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								builder->CreateBr(errorBlock);
							}, true);
					},
					[right, retVals, retBlocks, &retCount, errorBlock, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWFloat> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
						[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval, retVals, retBlocks, &retCount, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								retVals[retCount] = CompileFloatFloat(builder, env, lineno, lval, builder->CreateSIToFP(lval, FLOATTYPE, "rightToFloat"));
								retBlocks[retCount] = builder->GetInsertBlock();
								retCount++;
							},
							[lval, retVals, retBlocks, &retCount, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								retVals[retCount] = CompileFloatFloat(builder, env, lineno, lval, rval);
								retBlocks[retCount] = builder->GetInsertBlock();
								retCount++;
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								builder->CreateBr(errorBlock);
							}, true);
					},
					[right, retVals, retBlocks, &retCount, errorBlock, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWBool> lval) -> void {
						right->GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
						[errorBlock](NomBuilder& builder, RTPWValuePtr<PWRefValue> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWInt64> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[errorBlock](NomBuilder& builder, RTPWValuePtr<PWFloat> rval) -> void {
								builder->CreateBr(errorBlock);
							},
							[lval, retVals, retBlocks, &retCount, env, lineno, this](NomBuilder& builder, RTPWValuePtr<PWBool> rval) -> void {
								retVals[retCount] = CompileBoolBool(builder, env, lineno, lval, rval);
								retBlocks[retCount] = builder->GetInsertBlock();
								retCount++;
							}, true);
					},
					true);

				if (retCount > 1)
				{
					BasicBlock* outBlock = BasicBlock::Create(builder->getContext(), "binOpOut", env->Function);
					builder->SetInsertPoint(outBlock);
					PWPhi<PWPacked> phi = PWPhi<PWPacked>::CreatePtr(builder, retCount, "binOpPhi");
					for (int i = 0; i < retCount; i++)
					{
						builder->SetInsertPoint(retBlocks[i]);
						auto packed = retVals[i]->AsPackedValue(builder);
						builder->CreateBr(outBlock);
						phi->addIncoming(packed, builder->GetInsertBlock());
					}
					RegisterValue(env, RTValue::GetValue(builder, phi, NomDynamicType::DynamicRef));
					builder->SetInsertPoint(outBlock);
				}
				else if (retCount <= 0)
				{
					throw new std::exception();
				}
				else
				{
					builder->SetInsertPoint(retBlocks[0]);
					RegisterValue(env, retVals[0]);
				}
			}
		}

		void BinOpInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "BinOp #";
			cout << std::dec << Left;
			cout << " " << GetBinOpName(Operation) << " #";
			cout << std::dec << Right;
			cout << " -> #" << std::dec << WriteRegister;
			cout << "\n";
		}

		void BinOpInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}

	
		RTValuePtr BinOpInstruction::CompileIntInt(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::Add:
				return RTValue::GetValue(builder, builder->CreateAdd(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Subtract:
				return RTValue::GetValue(builder, builder->CreateSub(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Multiply:
				return RTValue::GetValue(builder, builder->CreateMul(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Divide:
				return RTValue::GetValue(builder, builder->CreateSDiv(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::Mod:
				return RTValue::GetValue(builder, builder->CreateSRem(left, right), NomIntClass::GetInstance()->GetType());
			case BinaryOperation::GreaterThan:
				return RTValue::GetValue(builder, builder->CreateICmpSGT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessThan:
				return RTValue::GetValue(builder, builder->CreateICmpSLT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::GreaterOrEqualTo:
				return RTValue::GetValue(builder, builder->CreateICmpSGE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessOrEqualTo:
				return RTValue::GetValue(builder, builder->CreateICmpSLE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			default:
			{
				auto nullVal = PoisonValue::get(REFTYPE);
				return RTValue::GetValue(builder, nullVal, NomType::AnythingRef);
			}
			}
		}
		RTValuePtr BinOpInstruction::CompileFloatFloat(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::Add:
				return RTValue::GetValue(builder, builder->CreateFAdd(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Subtract:
				return RTValue::GetValue(builder, builder->CreateFSub(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Multiply:
				return RTValue::GetValue(builder, builder->CreateFMul(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Divide:
				return RTValue::GetValue(builder, builder->CreateFDiv(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return RTValue::GetValue(builder, builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return RTValue::GetValue(builder, builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Mod:
				return RTValue::GetValue(builder, builder->CreateFRem(left, right), NomFloatClass::GetInstance()->GetType());
			case BinaryOperation::GreaterThan:
				return RTValue::GetValue(builder, builder->CreateFCmpOGT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessThan:
				return RTValue::GetValue(builder, builder->CreateFCmpOLT(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::GreaterOrEqualTo:
				return RTValue::GetValue(builder, builder->CreateFCmpOGE(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::LessOrEqualTo:
				return RTValue::GetValue(builder, builder->CreateFCmpOLE(left, right), NomBoolClass::GetInstance()->GetType());
			default:
			{
				auto nullVal = PoisonValue::get(REFTYPE);
				return RTValue::GetValue(builder, nullVal, NomType::AnythingRef);
			}
			}
		}
		RTValuePtr BinOpInstruction::CompileBoolBool(NomBuilder& builder, [[maybe_unused]] CompileEnv* env, [[maybe_unused]] size_t lineno, llvm::Value* left, llvm::Value* right)
		{
			switch (Operation)
			{
			case BinaryOperation::And:
				return RTValue::GetValue(builder, builder->CreateAnd(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Or:
				return RTValue::GetValue(builder, builder->CreateOr(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::BitXOR:
				return RTValue::GetValue(builder, builder->CreateXor(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::Equals:
				return RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			case BinaryOperation::RefEquals:
				return RTValue::GetValue(builder, builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType());
			default:
			{
				auto nullVal = PoisonValue::get(REFTYPE);
				return RTValue::GetValue(builder, nullVal, NomType::AnythingRef);
			}
			}
		}

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
