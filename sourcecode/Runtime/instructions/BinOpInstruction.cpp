#include "BinOpInstruction.h"
#include "llvm/IR/BasicBlock.h"
#include "../Common/Context.h"
#include "../Library/FloatClass.h"
#include "../Library/IntClass.h"
#include "../Library/BoolClass.h"
#include "../Intermediate_Representation/Types/NomClassType.h"
#include "../Common/TypeOperations.h"
#include "../Common/CompileHelpers.h"
#include <iostream>
#include "../Common/TypeOperations.h"
#include "CastInstruction.h"
#include "../Intermediate_Representation/NomValue.h"
#include "../Runtime_Data/Headers/RefValueHeader.h"
#include "../Runtime/RTOutput.h"
#include "../Common/Metadata.h"

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

		void BinOpInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			static auto boolType = NomBoolClass::GetInstance()->GetType();
			if (this->Operation == BinaryOperation::RefEquals || this->Operation == BinaryOperation::Equals)
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
						if (lefttype->IsDisjoint(righttype) || (lefttype->IsDisjoint(inttype) && lefttype->IsDisjoint(floattype)) || (righttype->IsDisjoint(inttype) && righttype->IsDisjoint(floattype)))
						{
							RegisterValue(env, NomValue(baseeq, NomBoolClass::GetInstance()->GetType()));
							return;
						}
						llvm::BasicBlock* nrefeqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NREFEQ", env->Function);
						neqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NEQ", env->Function);
						eqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "EQ", env->Function);
						builder->CreateCondBr(baseeq, eqblock, nrefeqblock);
						builder->SetInsertPoint(nrefeqblock);
					}
					else if (left->getType() == INTTYPE)
					{
						RegisterValue(env, NomValue(builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else if (left->getType() == FLOATTYPE)
					{
						RegisterValue(env, NomValue(builder->CreateFCmpOEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else if (left->getType() == BOOLTYPE)
					{
						RegisterValue(env, NomValue(builder->CreateICmpEQ(left, right), NomBoolClass::GetInstance()->GetType()));
						return;
					}
					else
					{
						std::cout << "Internal error: invalid binary operand types";
						throw new std::exception();
					}
				}
				if (neqblock == nullptr)
				{
					neqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "NEQ", env->Function);
				}
				if (eqblock == nullptr)
				{
					eqblock = llvm::BasicBlock::Create(LLVMCONTEXT, "EQ", env->Function);
				}
				BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operands for primitive operation!");

				llvm::BasicBlock* startBlock = builder->GetInsertBlock();
				llvm::BasicBlock* outblock = llvm::BasicBlock::Create(LLVMCONTEXT, "REFEQOUT", env->Function);

				NomValue ret;

				llvm::BasicBlock* leftRefValueBlock = nullptr, * leftIntBlock = nullptr, * leftFloatBlock = nullptr, * leftPrimitiveIntBlock = nullptr, * leftPrimitiveFloatBlock = nullptr, * leftPrimitiveBoolBlock = nullptr;
				llvm::Value* leftPrimitiveInt = nullptr, * leftPrimitiveFloat = nullptr, * leftPrimitiveBool = nullptr;
				RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, left, &leftRefValueBlock, &leftIntBlock, &leftFloatBlock, false, &leftPrimitiveIntBlock, &leftPrimitiveInt, &leftPrimitiveFloatBlock, &leftPrimitiveFloat, &leftPrimitiveBoolBlock, &leftPrimitiveBool);

				if (leftRefValueBlock != nullptr)
				{
					builder->SetInsertPoint(leftRefValueBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr, * rightIntBlock = nullptr, * rightFloatBlock = nullptr, * rightPrimitiveIntBlock = nullptr, * rightPrimitiveFloatBlock = nullptr, * rightPrimitiveBoolBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr, * rightPrimitiveBool = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &rightIntBlock, &rightFloatBlock, false, &rightPrimitiveIntBlock, &rightPrimitiveInt, &rightPrimitiveFloatBlock, &rightPrimitiveFloat, &rightPrimitiveBoolBlock, &rightPrimitiveBool);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);

						BasicBlock* vtableMatchBlock = BasicBlock::Create(LLVMCONTEXT, "vtablesMatchForEq", env->Function);
						BasicBlock* vtableMisMatchBlock = neqblock;
						if (!((lefttype->IsDisjoint(NomIntClass::GetInstance()->GetType()) && righttype->IsDisjoint(NomIntClass::GetInstance()->GetType())) ||
							(lefttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()) && righttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()))))
						{
							vtableMisMatchBlock = BasicBlock::Create(LLVMCONTEXT, "vtablesMismatchForEq", env->Function);
						}
						auto leftvtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
						auto rightvtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
						auto vtablesEq = CreatePointerEq(builder, leftvtable, rightvtable);
						Value* leftIsInt = nullptr;
						Value* leftIsFloat = nullptr;
						if (!lefttype->IsDisjoint(NomIntClass::GetInstance()->GetType()))
						{
							leftIsInt = CreatePointerEq(builder, leftvtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
						}
						else
						{
							leftIsFloat = CreatePointerEq(builder, leftvtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
						}
						CreateExpect(builder, vtablesEq, MakeIntLike(vtablesEq, 1));
						builder->CreateCondBr(vtablesEq, vtableMatchBlock, vtableMisMatchBlock, GetLikelyFirstBranchMetadata());

						builder->SetInsertPoint(vtableMatchBlock);
						{
							if (leftIsInt != nullptr)
							{
								BasicBlock* boxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "boxedInts", env->Function);
								BasicBlock* notBoxedIntBlock = neqblock;
								if (!lefttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()))
								{
									notBoxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "notBoxedInts", env->Function);
								}
								builder->CreateCondBr(leftIsInt, boxedIntBlock, notBoxedIntBlock);

								builder->SetInsertPoint(boxedIntBlock);
								{
									auto leftInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE);
									auto rightInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE);
									builder->CreateCondBr(builder->CreateICmpEQ(leftInt, rightInt), eqblock, neqblock);
								}

								builder->SetInsertPoint(notBoxedIntBlock);
							}
							if (!lefttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()))
							{
								BasicBlock* boxedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "boxedFloats", env->Function);
								if (leftIsFloat == nullptr)
								{
									leftIsFloat = CreatePointerEq(builder, leftvtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
								}
								CreateExpect(builder, leftIsFloat, MakeIntLike(leftIsFloat, 1));
								builder->CreateCondBr(leftIsFloat, boxedFloatBlock, neqblock, GetLikelyFirstBranchMetadata());

								builder->SetInsertPoint(boxedFloatBlock);
								{
									auto leftFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
									auto rightFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
									builder->CreateCondBr(builder->CreateFCmpOEQ(leftFloat, rightFloat), eqblock, neqblock);
								}
							}
						}
						if (vtableMisMatchBlock != neqblock)
						{
							builder->SetInsertPoint(vtableMisMatchBlock);
							if (leftIsInt != nullptr)
							{
								BasicBlock* boxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "leftBoxedInt", env->Function);
								BasicBlock* notBoxedIntBlock = neqblock;
								if (!lefttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()))
								{
									notBoxedIntBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotBoxedInt", env->Function);
								}
								builder->CreateCondBr(leftIsInt, boxedIntBlock, notBoxedIntBlock);

								builder->SetInsertPoint(boxedIntBlock);
								{
									BasicBlock* rightBoxedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "rightBoxedFloat", env->Function);
									auto rightIsFloat = CreatePointerEq(builder, rightvtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
									builder->CreateCondBr(rightIsFloat, rightBoxedFloatBlock, neqblock);

									builder->SetInsertPoint(rightBoxedFloatBlock);
									auto leftFloat = builder->CreateSIToFP(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
									auto rightFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
									builder->CreateCondBr(builder->CreateFCmpOEQ(leftFloat, rightFloat), eqblock, neqblock);
								}

								builder->SetInsertPoint(notBoxedIntBlock);
							}
							if (!lefttype->IsDisjoint(NomFloatClass::GetInstance()->GetType()))
							{
								BasicBlock* boxedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "leftBoxedFloat", env->Function);
								auto leftIsFloat = CreatePointerEq(builder, leftvtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
								auto rightIsInt = CreatePointerEq(builder, rightvtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
								auto floatAndInt = builder->CreateAnd(leftIsFloat, rightIsInt);
								CreateExpect(builder, floatAndInt, MakeIntLike(leftIsFloat, 1));
								builder->CreateCondBr(floatAndInt, boxedFloatBlock, neqblock, GetLikelyFirstBranchMetadata());

								builder->SetInsertPoint(boxedFloatBlock);
								{
									auto leftFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
									auto rightFloat = builder->CreateSIToFP(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
									builder->CreateCondBr(builder->CreateFCmpOEQ(leftFloat, rightFloat), eqblock, neqblock);
								}
							}
						}
					}

					if (rightIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightIntBlock);
						if (!lefttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntObjRightInt", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotIntRightInt", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							leftPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE);
							auto intEq = builder->CreateICmpEQ(leftPrimitiveInt, UnpackMaskedInt(builder, right));
							builder->CreateCondBr(intEq, eqblock, neqblock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						if (!lefttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightInt", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightInt", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							leftPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, builder->CreateSIToFP(UnpackMaskedInt(builder, right), FLOATTYPE));
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightFloatBlock);
						if (!lefttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightFloat", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightFloat", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							leftPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, UnpackMaskedFloat(builder, right));
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						if (!lefttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntObjRightFloat", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotIntRightFloat", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							leftPrimitiveFloat = builder->CreateSIToFP(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, UnpackMaskedFloat(builder, right));
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightPrimitiveIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveIntBlock);
						if (!lefttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntObjRightPrimitiveInt", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotIntRightPrimitiveInt", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							leftPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE);
							auto intEq = builder->CreateICmpEQ(leftPrimitiveInt, rightPrimitiveInt);
							builder->CreateCondBr(intEq, eqblock, neqblock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						if (!lefttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightPrimitiveInt", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotFloatObjRightPrimitiveInt", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							leftPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, builder->CreateSIToFP(rightPrimitiveInt, FLOATTYPE));
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightPrimitiveFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveFloatBlock);
						if (!lefttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatObjRightPrimitiveFloat", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotFloatObjRightPrimitiveFloat", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							leftPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, rightPrimitiveFloat);
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						if (!lefttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntObjRightPrimitiveFloat", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftNotIntRightPrimitiveFloat", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, left);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							leftPrimitiveFloat = builder->CreateSIToFP(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, left, 0, false), INTTYPE), FLOATTYPE);
							auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, rightPrimitiveFloat);
							builder->CreateCondBr(floatEq, eqblock, neqblock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						builder->CreateBr(neqblock);

					}

					if (rightPrimitiveBoolBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveBoolBlock);
						auto boolEq = CreatePointerEq(builder, left, NomBoolObjects::PackBool(builder, rightPrimitiveBool));
						builder->CreateCondBr(boolEq, eqblock, neqblock);
					}
				}

				if (leftIntBlock != nullptr)
				{
					builder->SetInsertPoint(leftIntBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr, * rightFloatBlock = nullptr, * rightPrimitiveIntBlock = nullptr, * rightPrimitiveFloatBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr, * rightPrimitiveBool = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &neqblock, &rightFloatBlock, false, &rightPrimitiveIntBlock, &rightPrimitiveInt, &rightPrimitiveFloatBlock, &rightPrimitiveFloat, &errorBlock, &rightPrimitiveBool, 30, 100, 20, 1);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);
						if (!righttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightIntObj", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightNotIntObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							if (rightPrimitiveIntBlock != nullptr)
							{
								std::cout << "Internal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveIntBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightPrimitiveInt", env->Function);
							rightPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE);
							builder->CreateBr(rightPrimitiveIntBlock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						if (!righttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightFloatObj", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightNotFloatObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							if (rightPrimitiveFloatBlock != nullptr)
							{
								std::cout << "Internal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveFloatBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightPrimitiveFloat", env->Function);
							rightPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
							builder->CreateBr(rightPrimitiveFloatBlock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightFloatBlock);
						auto leftFloat = builder->CreateSIToFP(UnpackMaskedInt(builder, builder->CreatePtrToInt(left, INTTYPE)), FLOATTYPE);
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, UnpackMaskedFloat(builder, builder->CreatePtrToInt(right, INTTYPE)));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightPrimitiveIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveIntBlock);
						auto leftInt = UnpackMaskedInt(builder, builder->CreatePtrToInt(left, INTTYPE));
						auto intEq = builder->CreateICmpEQ(leftInt, rightPrimitiveInt);
						builder->CreateCondBr(intEq, eqblock, neqblock);
					}
					if (rightPrimitiveFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveFloatBlock);
						auto leftFloat = builder->CreateSIToFP(UnpackMaskedInt(builder, builder->CreatePtrToInt(left, INTTYPE)), FLOATTYPE);
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, rightPrimitiveFloat);
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
				}

				if (leftFloatBlock != nullptr)
				{
					builder->SetInsertPoint(leftFloatBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr, * rightIntBlock = nullptr, * rightFloatBlock = nullptr, * rightPrimitiveIntBlock = nullptr, * rightPrimitiveFloatBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr, * rightPrimitiveBool = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &rightIntBlock, &rightFloatBlock, false, &rightPrimitiveIntBlock, &rightPrimitiveInt, &rightPrimitiveFloatBlock, &rightPrimitiveFloat, &errorBlock, &rightPrimitiveBool, 30, 20, 100, 1);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);
						if (!righttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightFloatObj", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightNotFloatObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							if (rightPrimitiveFloatBlock != nullptr)
							{
								std::cout << "Floaternal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveFloatBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightPrimitiveFloat", env->Function);
							rightPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
							builder->CreateBr(rightPrimitiveFloatBlock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						if (!righttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightIntObj", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightNotIntObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							if (rightPrimitiveIntBlock != nullptr)
							{
								std::cout << "Internal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveIntBlock = BasicBlock::Create(LLVMCONTEXT, "leftFloatRightPrimitiveInt", env->Function);
							rightPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE);
							builder->CreateBr(rightPrimitiveIntBlock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightIntBlock);
						auto leftFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(left, INTTYPE));
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, builder->CreateSIToFP(UnpackMaskedInt(builder, builder->CreatePtrToInt(right, INTTYPE)), FLOATTYPE));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightFloatBlock);
						auto leftFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(left, INTTYPE));
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, UnpackMaskedFloat(builder, builder->CreatePtrToInt(right, INTTYPE)));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightPrimitiveIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveIntBlock);
						auto leftFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(left, INTTYPE));
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, builder->CreateSIToFP(rightPrimitiveInt, FLOATTYPE));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightPrimitiveFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveFloatBlock);
						auto leftFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(left, INTTYPE));
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, rightPrimitiveFloat);
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
				}

				if (leftPrimitiveIntBlock != nullptr)
				{
					builder->SetInsertPoint(leftPrimitiveIntBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr, * rightFloatBlock = nullptr, * rightIntBlock = nullptr, * rightPrimitiveFloatBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr, * rightPrimitiveBool = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &rightIntBlock, &rightFloatBlock, false, &neqblock, &rightPrimitiveInt, &rightPrimitiveFloatBlock, &rightPrimitiveFloat, &errorBlock, &rightPrimitiveBool, 30, 100, 20, 1);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);
						if (!righttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightIntObj", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightNotIntObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							rightPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE);
							auto intEq = builder->CreateICmpEQ(rightPrimitiveInt, leftPrimitiveInt);
							builder->CreateCondBr(intEq, eqblock, neqblock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						if (!righttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightFloatObj", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightNotFloatObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							if (rightPrimitiveFloatBlock != nullptr)
							{
								std::cout << "Internal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveFloatBlock = BasicBlock::Create(LLVMCONTEXT, "leftIntRightPrimitiveFloat", env->Function);
							rightPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
							builder->CreateBr(rightPrimitiveFloatBlock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightFloatBlock);
						auto leftFloat = builder->CreateSIToFP(leftPrimitiveInt, FLOATTYPE);
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, UnpackMaskedFloat(builder, builder->CreatePtrToInt(right, INTTYPE)));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightIntBlock);
						auto rightInt = UnpackMaskedInt(builder, builder->CreatePtrToInt(right, INTTYPE));
						auto intEq = builder->CreateICmpEQ(rightInt, leftPrimitiveInt);
						builder->CreateCondBr(intEq, eqblock, neqblock);
					}
					if (rightPrimitiveFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveFloatBlock);
						auto leftFloat = builder->CreateSIToFP(leftPrimitiveInt, FLOATTYPE);
						auto floatEq = builder->CreateFCmpOEQ(leftFloat, rightPrimitiveFloat);
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
				}

				if (leftPrimitiveFloatBlock != nullptr)
				{
					builder->SetInsertPoint(leftPrimitiveFloatBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr, * rightFloatBlock = nullptr, * rightIntBlock = nullptr, * rightPrimitiveIntBlock = nullptr, * rightPrimitiveFloatBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr, * rightPrimitiveBool = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &rightIntBlock, &rightFloatBlock, false, &rightPrimitiveIntBlock, &rightPrimitiveInt, &rightPrimitiveFloatBlock, &rightPrimitiveFloat, &errorBlock, &rightPrimitiveBool, 30, 20, 100, 1);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);
						if (!righttype->IsDisjoint(floattype))
						{
							BasicBlock* floatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightFloatObj", env->Function);
							BasicBlock* notFloatObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightNotFloatObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isFloat = CreatePointerEq(builder, vtable, NomFloatClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isFloat, MakeIntLike(isFloat, 1));
							builder->CreateCondBr(isFloat, floatObjBlock, notFloatObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(floatObjBlock);
							if (rightPrimitiveFloatBlock != nullptr)
							{
								std::cout << "Floaternal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveFloatBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightPrimitiveFloat", env->Function);
							rightPrimitiveFloat = builder->CreateBitCast(builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE), FLOATTYPE);
							builder->CreateBr(rightPrimitiveFloatBlock);

							builder->SetInsertPoint(notFloatObjBlock);
						}
						if (!righttype->IsDisjoint(inttype))
						{
							BasicBlock* intObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightIntObj", env->Function);
							BasicBlock* notIntObjBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightNotIntObj", env->Function);
							auto vtable = RefValueHeader::GenerateReadVTablePointer(builder, right);
							auto isInt = CreatePointerEq(builder, vtable, NomIntClass::GetInstance()->GetType()->GetLLVMElement(*env->Module));
							CreateExpect(builder, isInt, MakeIntLike(isInt, 1));
							builder->CreateCondBr(isInt, intObjBlock, notIntObjBlock, GetLikelyFirstBranchMetadata());

							builder->SetInsertPoint(intObjBlock);
							if (rightPrimitiveIntBlock != nullptr)
							{
								std::cout << "Internal error: wrong primitive case generation!";
								throw new std::exception();
							}
							rightPrimitiveIntBlock = BasicBlock::Create(LLVMCONTEXT, "leftPrimitiveFloatRightPrimitiveInt", env->Function);
							rightPrimitiveInt = builder->CreatePtrToInt(ObjectHeader::ReadField(builder, right, 0, false), INTTYPE);
							builder->CreateBr(rightPrimitiveIntBlock);

							builder->SetInsertPoint(notIntObjBlock);
						}
						builder->CreateBr(neqblock);
					}
					if (rightFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightFloatBlock);
						auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, UnpackMaskedFloat(builder, builder->CreatePtrToInt(right, INTTYPE)));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightIntBlock);
						auto rightFloat = builder->CreateSIToFP(UnpackMaskedInt(builder, builder->CreatePtrToInt(right, INTTYPE)), FLOATTYPE);
						auto floatEq = builder->CreateFCmpOEQ(rightFloat, leftPrimitiveFloat);
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightPrimitiveIntBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveIntBlock);
						auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, builder->CreateSIToFP(rightPrimitiveFloat, FLOATTYPE));
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
					if (rightPrimitiveFloatBlock != nullptr)
					{
						builder->SetInsertPoint(rightPrimitiveFloatBlock);
						auto floatEq = builder->CreateFCmpOEQ(leftPrimitiveFloat, rightPrimitiveFloat);
						builder->CreateCondBr(floatEq, eqblock, neqblock);
					}
				}

				if (leftPrimitiveBoolBlock != nullptr)
				{
					builder->SetInsertPoint(leftPrimitiveBoolBlock);
					llvm::BasicBlock* rightRefValueBlock = nullptr;
					llvm::Value* rightPrimitiveInt = nullptr, * rightPrimitiveFloat = nullptr;
					RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, right, &rightRefValueBlock, &neqblock, &neqblock, false, &neqblock, &rightPrimitiveInt, &neqblock, &rightPrimitiveFloat, nullptr, nullptr, 30, 1, 1, 100);

					if (rightRefValueBlock != nullptr)
					{
						builder->SetInsertPoint(rightRefValueBlock);
						auto boolEq = CreatePointerEq(builder, right, NomBoolObjects::PackBool(builder, leftPrimitiveBool));
						builder->CreateCondBr(boolEq, eqblock, neqblock);
					}
				}

				builder->SetInsertPoint(eqblock);
				builder->CreateBr(outblock);

				builder->SetInsertPoint(neqblock);
				builder->CreateBr(outblock);

				builder->SetInsertPoint(outblock);
				auto outPHI = builder->CreatePHI(inttype(1), 2);
				outPHI->addIncoming(MakeUInt(1, 1), eqblock);
				outPHI->addIncoming(MakeUInt(1, 0), neqblock);

				RegisterValue(env, NomValue(outPHI, NomBoolClass::GetInstance()->GetType()));
			}
			else
			{
				llvm::Value* leftTag;
				NomTypeRef leftType;
				llvm::Value* leftValue = env->LookupUnwrapped(Left, &leftTag, &leftType);

				llvm::Value* rightTag;
				NomTypeRef rightType;
				llvm::Value* rightValue = env->LookupUnwrapped(Right, &rightTag, &rightType);

				llvm::Value* tagsPaired = builder->CreateOr(builder->CreateShl(
					builder->CreateZExt(leftTag, llvm::Type::getIntNTy(LLVMCONTEXT, 4)), 2), 
					builder->CreateZExt(rightTag, llvm::Type::getIntNTy(LLVMCONTEXT, 4)));
				Function* fun = builder->GetInsertBlock()->getParent();
				BasicBlock* failBlock = 
				RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid binary operation on non-primitive operand!");
				SwitchInst* switchInst = builder->CreateSwitch(tagsPaired, failBlock, 2);

				BasicBlock* outBlock = BasicBlock::Create(LLVMCONTEXT, "outputBinOp", fun);
				builder->SetInsertPoint(outBlock);
				llvm::Type* unpackedTy = llvm::Type::getInt64Ty(LLVMCONTEXT);
				llvm::Type* tagTy = llvm::Type::getIntNTy(LLVMCONTEXT, 2);
				PHINode* outValue = builder->CreatePHI(unpackedTy, 2);
				PHINode* outTag = builder->CreatePHI(tagTy, 2);

				BasicBlock* finalFloatFloatBlock = BasicBlock::Create(LLVMCONTEXT, "finalFloatFloatBinOp", fun);

				BasicBlock* floatFloatBlock = BasicBlock::Create(LLVMCONTEXT, "floatFloatBinOp", fun);
				builder->SetInsertPoint(floatFloatBlock);
				llvm::Value* leftFloatFloat = builder->CreateBitCast(leftValue, FLOATTYPE, "leftFloat");
				llvm::Value* rightFloatFloat = builder->CreateBitCast(rightValue, FLOATTYPE, "rightFloat");
				builder->CreateBr(finalFloatFloatBlock);

				BasicBlock* intFloatBlock = BasicBlock::Create(LLVMCONTEXT, "intFloatBinOp", fun);
				builder->SetInsertPoint(intFloatBlock);
				llvm::Value* leftIntFloat = builder->CreateSIToFP(builder->CreateBitCast(leftValue, INTTYPE), FLOATTYPE, "leftFloat");
				llvm::Value* rightIntFloat = builder->CreateBitCast(rightValue, FLOATTYPE, "rightFloat");
				builder->CreateBr(finalFloatFloatBlock);

				BasicBlock* floatIntBlock = BasicBlock::Create(LLVMCONTEXT, "floatIntBinOp", fun);
				builder->SetInsertPoint(floatIntBlock);
				llvm::Value* leftFloatInt = builder->CreateBitCast(leftValue, FLOATTYPE, "leftFloat");
				llvm::Value* rightFloatInt = builder->CreateSIToFP(builder->CreateBitCast(rightValue, INTTYPE), FLOATTYPE, "rightFloat");
				builder->CreateBr(finalFloatFloatBlock);

				builder->SetInsertPoint(finalFloatFloatBlock);
				PHINode* leftFloatPhi = builder->CreatePHI(FLOATTYPE, 3, "leftFloatPhi");
				leftFloatPhi->addIncoming(leftFloatFloat, floatFloatBlock);
				leftFloatPhi->addIncoming(leftIntFloat, intFloatBlock);
				leftFloatPhi->addIncoming(leftFloatInt, floatIntBlock);
				PHINode* rightFloatPhi = builder->CreatePHI(FLOATTYPE, 3, "rightFloatPhi");
				rightFloatPhi->addIncoming(rightFloatFloat, floatFloatBlock);
				rightFloatPhi->addIncoming(rightIntFloat, intFloatBlock);
				rightFloatPhi->addIncoming(rightFloatInt, floatIntBlock);


				llvm::Value* floatTagResult;
				llvm::Value* floatResult = CompileFloatFloatLLVM(builder, env, lineno, leftFloatPhi, rightFloatPhi, &floatTagResult);
				if (floatResult == nullptr) {
					BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
					builder->CreateBr(invalidBlock);
				} else {
					outValue->addIncoming(builder->CreateZExtOrBitCast(floatResult, unpackedTy), finalFloatFloatBlock);
					outTag->addIncoming(floatTagResult, finalFloatFloatBlock);
					builder->CreateBr(outBlock);
				}

				BasicBlock* intIntBlock = BasicBlock::Create(LLVMCONTEXT, "intIntBinOp", fun);
				builder->SetInsertPoint(intIntBlock);
				llvm::Value* leftInt = builder->CreateBitCast(leftValue, INTTYPE, "leftInt");
				llvm::Value* rightInt = builder->CreateBitCast(rightValue, INTTYPE, "rightInt");
				llvm::Value* intTagResult;
				llvm::Value* intResult = CompileIntIntLLVM(builder, env, lineno, leftInt, rightInt, &intTagResult);
				if (intResult == nullptr) {
					BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
					builder->CreateBr(invalidBlock);
				} else {
					outValue->addIncoming(builder->CreateZExtOrBitCast(intResult, unpackedTy), intIntBlock);
					outTag->addIncoming(intTagResult, intIntBlock);
					builder->CreateBr(outBlock);
				}

				BasicBlock* boolBoolBlock = BasicBlock::Create(LLVMCONTEXT, "boolBoolBinOp", fun);
				builder->SetInsertPoint(boolBoolBlock);
				llvm::Value* leftBool = builder->CreateTrunc(leftValue, BOOLTYPE, "leftBool");
				llvm::Value* rightBool = builder->CreateTrunc(rightValue, BOOLTYPE, "rightBool");
				llvm::Value* boolTagResult;
				llvm::Value* boolResult = CompileBoolBoolLLVM(builder, env, lineno, leftBool, rightBool, &boolTagResult);
				if (boolResult == nullptr) {
					BasicBlock* invalidBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Invalid operation on given operands!");
					builder->CreateBr(invalidBlock);
				} else {
					outValue->addIncoming(builder->CreateZExt(boolResult, unpackedTy), boolBoolBlock);
					outTag->addIncoming(boolTagResult, boolBoolBlock);
					builder->CreateBr(outBlock);
				}

				switchInst->addCase(MakeUInt(4, 1 << 2 | 1), floatFloatBlock);
				switchInst->addCase(MakeUInt(4, 3 << 2 | 1), intFloatBlock);
				switchInst->addCase(MakeUInt(4, 1 << 2 | 3), floatIntBlock);
				switchInst->addCase(MakeUInt(4, 3 << 2 | 3), intIntBlock);
				switchInst->addCase(MakeUInt(4, 2 << 2 | 2), boolBoolBlock);

				BasicBlock* packBlock = BasicBlock::Create(LLVMCONTEXT, "packBlock", fun);
				builder->SetInsertPoint(outBlock);
				builder->CreateBr(packBlock);

				BasicBlock* continueBlock = BasicBlock::Create(LLVMCONTEXT, "afterBinOp", fun);
				builder->SetInsertPoint(packBlock);
				builder->CreateBr(continueBlock);

				auto outVal = NomValue(outValue, outTag, packBlock, NomReturnType(leftType, rightType));

				builder->SetInsertPoint(continueBlock);
				RegisterValue(env, outVal);
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

		llvm::Value* BinOpInstruction::CompileIntIntLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag)
		{
			llvm::Value* intTypeTag = MakeUInt(2, 3);
			llvm::Value* boolTypeTag = MakeUInt(2, 2);
			switch (Operation)
			{
			case BinaryOperation::Add:
				*resultTag = intTypeTag;
				return builder->CreateAdd(left, right);
			case BinaryOperation::Subtract:
				*resultTag = intTypeTag;
				return builder->CreateSub(left, right);
			case BinaryOperation::Multiply:
				*resultTag = intTypeTag;
				return builder->CreateMul(left, right);
			case BinaryOperation::Divide:
				*resultTag = intTypeTag;
				return builder->CreateSDiv(left, right);
			case BinaryOperation::Mod:
				*resultTag = intTypeTag;
				return builder->CreateSRem(left, right);
			case BinaryOperation::GreaterThan:
				*resultTag = boolTypeTag;
				return builder->CreateICmpSGT(left, right);
			case BinaryOperation::LessThan:
				*resultTag = boolTypeTag;
				return builder->CreateICmpSLT(left, right);
			case BinaryOperation::GreaterOrEqualTo:
				*resultTag = boolTypeTag;
				return builder->CreateICmpSGE(left, right);
			case BinaryOperation::LessOrEqualTo:
				*resultTag = boolTypeTag;
				return builder->CreateICmpSLE(left, right);
			case BinaryOperation::Equals:
				*resultTag = boolTypeTag;
				return builder->CreateICmpEQ(left, right);
			case BinaryOperation::RefEquals:
				*resultTag = boolTypeTag;
				return builder->CreateICmpEQ(left, right);
			default:
			{
				*resultTag = nullptr;
				return nullptr;
			}
			}
		}
		llvm::Value* BinOpInstruction::CompileFloatFloatLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag)
		{
			llvm::Value* floatTypeTag = MakeUInt(2, 1);
			llvm::Value* boolTypeTag = MakeUInt(2, 2);
			switch (Operation)
			{
			case BinaryOperation::Add:
				*resultTag = floatTypeTag;
				return builder->CreateFAdd(left, right);
			case BinaryOperation::Subtract:
				*resultTag = floatTypeTag;
				return builder->CreateFSub(left, right);
			case BinaryOperation::Multiply:
				*resultTag = floatTypeTag;
				return builder->CreateFMul(left, right);
			case BinaryOperation::Divide:
				*resultTag = floatTypeTag;
				return builder->CreateFDiv(left, right);
			case BinaryOperation::Mod:
				*resultTag = floatTypeTag;
				return builder->CreateFRem(left, right);
			case BinaryOperation::Equals:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOEQ(left, right);
			case BinaryOperation::RefEquals:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOEQ(left, right);
			case BinaryOperation::GreaterThan:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOGT(left, right);
			case BinaryOperation::LessThan:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOLT(left, right);
			case BinaryOperation::GreaterOrEqualTo:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOGE(left, right);
			case BinaryOperation::LessOrEqualTo:
				*resultTag = boolTypeTag;
				return builder->CreateFCmpOLE(left, right);
			default:
			{
				*resultTag = nullptr;
				return nullptr;
			}
			}
		}
		llvm::Value* BinOpInstruction::CompileBoolBoolLLVM(NomBuilder& builder, CompileEnv* env, int lineno, llvm::Value* left, llvm::Value* right, llvm::Value** resultTag)
		{
			*resultTag = MakeUInt(2, 2);
			switch (Operation)
			{
			case BinaryOperation::And:
				return builder->CreateAnd(left, right);
			case BinaryOperation::Or:
				return builder->CreateOr(left, right);
			case BinaryOperation::BitXOR:
				return builder->CreateXor(left, right);
			case BinaryOperation::Equals:
				return builder->CreateICmpEQ(left, right);
			case BinaryOperation::RefEquals:
				return builder->CreateICmpEQ(left, right);
			default:
			{
				*resultTag = nullptr;
				return nullptr;
			}
			}
		}

		NomTypeRef BinOpInstruction::NomReturnType(NomTypeRef leftTy, NomTypeRef rightTy) {
			switch (Operation)
			{
			case BinaryOperation::And:
			case BinaryOperation::Or:
			case BinaryOperation::BitXOR:
			case BinaryOperation::GreaterThan:
			case BinaryOperation::LessThan:
			case BinaryOperation::GreaterOrEqualTo:
			case BinaryOperation::Equals:
			case BinaryOperation::RefEquals:
				return GetBoolClassType();
			case BinaryOperation::Add:
			case BinaryOperation::Subtract:
			case BinaryOperation::Multiply:
			case BinaryOperation::Divide:
			case BinaryOperation::Mod:
				if (leftTy->IsSubtype(GetFloatClassType(), false) || rightTy->IsSubtype(GetFloatClassType(), false))
					return GetFloatClassType();
				else if (leftTy->IsSubtype(GetIntClassType(), false) && rightTy->IsSubtype(GetIntClassType(), false))
					return GetIntClassType();
			default:
				break;
			}
			return GetDynamicType();
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
