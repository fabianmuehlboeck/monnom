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
#include "../Metadata.h"
#include "../PWRefValue.h"

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
				PWRefValue leftrv = left.AsPWRef();
				PWRefValue rightrv = right.AsPWRef();

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
						auto leftvtable = leftrv.ReadVTable(builder);
						auto rightvtable = rightrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = leftrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
							auto vtable = rightrv.ReadVTable(builder);
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
				auto nullVal = PoisonValue::get(REFTYPE);
				return NomValue(nullVal, NomType::Anything);
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
