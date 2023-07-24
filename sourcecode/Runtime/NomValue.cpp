#include "NomValue.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "NomClassType.h"
#include "BoolClass.h"
#include "NomDynamicType.h"
#include "CompileHelpers.h"
#include "Metadata.h"
#include "PWObject.h"
#include "PWRefValue.h"
#include "PWVTable.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		NomTypeRef GetIntClassType()
		{
			return NomIntClass::GetInstance()->GetType();
		}
		NomTypeRef GetFloatClassType()
		{
			return NomFloatClass::GetInstance()->GetType();
		}
		NomTypeRef GetBoolClassType()
		{
			return NomBoolClass::GetInstance()->GetType();
		}
		NomTypeRef GetDynamicType()
		{
			return NomDynamicType::DynamicRef;
		}
		NomTypeRef GetDynOrPrimitiveType(llvm::Value* val)
		{
			llvm::Type* valType = val->getType();
			if (valType->isIntegerTy(INTTYPE->getIntegerBitWidth()))
			{
				return GetIntClassType();
			}
			if (valType->isDoubleTy())
			{
				return GetFloatClassType();
			}
			if (valType->isIntegerTy(BOOLTYPE->getIntegerBitWidth()))
			{
				return GetBoolClassType();
			}
			return GetDynamicType();
		}
		void _nomValueContainerDebugCheck(NomTypeRef type, llvm::Type* valType)
		{
			if (type->IsSubtype(GetIntClassType()))
			{
				if (valType->isFloatingPointTy() || valType->isDoubleTy())
				{
					throw new std::exception();
				}
			}
			else if (type->IsSubtype(GetFloatClassType()))
			{
				if (valType->isIntegerTy(64))
				{
					throw new std::exception();
				}
			}
		}

		PWRefValue NomValue::operator->() const
		{
			return PWRefValue(val);
		}
		PWRefValue NomValue::AsPWRef() const
		{
			return PWRefValue(val);
		}
		int NomValue::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolVar, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			llvm::Type* valType = val->getType();
			if (valType->isIntegerTy(INTTYPE->getIntegerBitWidth()))
			{
				if (primitiveIntBlock == nullptr)
				{
					throw new std::exception();
				}
				*primitiveIntBlock = builder->GetInsertBlock();
				if (primitiveIntVar != nullptr)
				{
					*primitiveIntVar = val;
				}
				return 1;
			}
			if (valType->isDoubleTy())
			{
				if (primitiveFloatBlock == nullptr)
				{
					throw new std::exception();
				}
				*primitiveFloatBlock = builder->GetInsertBlock();
				if (primitiveFloatVar != nullptr)
				{
					*primitiveFloatVar = val;
				}
				return 1;
			}
			if (valType->isIntegerTy(BOOLTYPE->getIntegerBitWidth()))
			{
				if (primitiveBoolBlock == nullptr)
				{
					throw new std::exception();
				}
				*primitiveBoolBlock = builder->GetInsertBlock();
				if (primitiveBoolVar != nullptr)
				{
					*primitiveBoolVar = val;
				}
				return 1;
			}

			if (unpackPrimitives && GetNomType()->IsSubtype(GetIntClassType(), false))
			{
				if (primitiveIntBlock == nullptr || primitiveIntVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveIntVar = UnpackInt(builder, *this);
				*primitiveIntBlock = builder->GetInsertBlock();
				return 1;
			}
			else if (unpackPrimitives && GetNomType()->IsSubtype(GetFloatClassType(), false))
			{
				if (primitiveFloatBlock == nullptr || primitiveFloatVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveFloatVar = UnpackFloat(builder, *this);
				*primitiveFloatBlock = builder->GetInsertBlock();
				return 1;
			}
			else if (unpackPrimitives && GetNomType()->IsSubtype(GetBoolClassType(), false))
			{
				if (primitiveBoolBlock == nullptr || primitiveBoolVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveBoolVar = UnpackBool(builder, *this);
				*primitiveBoolBlock = builder->GetInsertBlock();
				return 1;
			}

			bool intsPossible = !GetNomType()->IsDisjoint(GetIntClassType());
			bool floatsPossible = !GetNomType()->IsDisjoint(GetFloatClassType());
			bool boolsPossible = !GetNomType()->IsDisjoint(GetFloatClassType());

			int cases = 1; //could always be normal reference at this point

			BasicBlock* _packedIntBlock = nullptr;
			BasicBlock* _primitiveIntBlock = nullptr;
			PHINode* _primitiveIntPHI = nullptr;
			if (intBlock != nullptr)
			{
				_packedIntBlock = *intBlock;
			}
			if (primitiveIntBlock != nullptr)
			{
				_primitiveIntBlock = *primitiveIntBlock;
			}

			if (intsPossible)
			{
				cases += 1;
				if (_packedIntBlock == nullptr)
				{
					_packedIntBlock = BasicBlock::Create(LLVMCONTEXT, "packedInt", fun);
					if (intBlock != nullptr)
					{
						*intBlock = _packedIntBlock;
					}
					else if (!unpackPrimitives)
					{
						std::cout << "Internal error: did not cover packed integer case";
						throw new std::exception();
					}
				}
				if (unpackPrimitives)
				{
					if (primitiveIntBlock == nullptr || primitiveIntVar == nullptr)
					{
						throw new std::exception();
					}
					if (_primitiveIntBlock == nullptr)
					{
						_primitiveIntBlock = BasicBlock::Create(LLVMCONTEXT, "primitiveInt", fun);
						*primitiveIntBlock = _primitiveIntBlock;
					}
					if (_primitiveIntPHI == nullptr)
					{
						builder->SetInsertPoint(_primitiveIntBlock);
						_primitiveIntPHI = builder->CreatePHI(INTTYPE, 2, "primitiveIntValue");
						*primitiveIntVar = _primitiveIntPHI;
					}
					builder->SetInsertPoint(_packedIntBlock);
					auto unpackedInt = UnpackMaskedInt(builder, builder->CreatePtrToInt(val, INTTYPE, "refAsInt"));
					_primitiveIntPHI->addIncoming(unpackedInt, builder->GetInsertBlock());
					builder->CreateBr(_primitiveIntBlock);
				}
			}

			BasicBlock* _maskedFloatBlock = nullptr;
			BasicBlock* _primitiveFloatBlock = nullptr;
			PHINode* _primitiveFloatPHI = nullptr;
			if (floatBlock != nullptr)
			{
				_maskedFloatBlock = *floatBlock;
			}
			if (primitiveFloatBlock != nullptr)
			{
				_primitiveFloatBlock = *primitiveFloatBlock;
			}

			if (floatsPossible)
			{
				cases += 1;
				if (_maskedFloatBlock == nullptr)
				{
					_maskedFloatBlock = BasicBlock::Create(LLVMCONTEXT, "packedFloat", fun);
					if (floatBlock != nullptr)
					{
						*floatBlock = _maskedFloatBlock;
					}
					else if (!unpackPrimitives)
					{
						std::cout << "Internal error: did not cover packed float case";
						throw new std::exception();
					}
				}
				if (unpackPrimitives)
				{
					if (primitiveFloatBlock == nullptr || primitiveFloatVar == nullptr)
					{
						throw new std::exception();
					}
					if (_primitiveFloatBlock == nullptr)
					{
						_primitiveFloatBlock = BasicBlock::Create(LLVMCONTEXT, "primitiveFloat", fun);
						*primitiveFloatBlock = _primitiveFloatBlock;
					}
					if (_primitiveFloatPHI == nullptr)
					{
						builder->SetInsertPoint(_primitiveFloatBlock);
						_primitiveFloatPHI = builder->CreatePHI(FLOATTYPE, 2, "primitiveFloatValue");
						*primitiveFloatVar = _primitiveFloatPHI;
					}
					builder->SetInsertPoint(_maskedFloatBlock);
					auto unpackedFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(val, INTTYPE, "refAsInt"));
					_primitiveFloatPHI->addIncoming(unpackedFloat, builder->GetInsertBlock());
					builder->CreateBr(_primitiveFloatBlock);
				}
			}

			if (refValueBlock == nullptr)
			{
				throw new std::exception();
			}

			llvm::Value* vtableAsInt = nullptr;
			if (cases == 1)
			{
				if ((!unpackPrimitives) || (!boolsPossible))
				{
					*refValueBlock = builder->GetInsertBlock();
					return 1;
				}
				else // booleans need to be unpacked to primitive values
				{
					if (primitiveBoolBlock == nullptr || primitiveBoolVar == nullptr)
					{
						throw new std::exception();
					}
					*refValueBlock = BasicBlock::Create(LLVMCONTEXT, "refValue", fun);
					*primitiveBoolBlock = BasicBlock::Create(LLVMCONTEXT, "boxedBooleanA", fun);
					vtableAsInt = builder->CreatePtrToInt(this->AsPWRef().ReadVTable(builder), numtype(intptr_t));
					auto boolAsInt = ConstantExpr::getPtrToInt(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isBool = builder->CreateICmpEQ(vtableAsInt, boolAsInt);
					builder->CreateCondBr(isBool, *primitiveBoolBlock, *refValueBlock);
					builder->SetInsertPoint(*primitiveBoolBlock);
					*primitiveBoolVar = UnpackBool(builder, val);
					return 2;
				}
			}
			builder->SetInsertPoint(origBlock);
			auto refAsInt = builder->CreatePtrToInt(val, INTTYPE, "refAsInt");
			auto tag = builder->CreateTrunc(refAsInt, inttype(2), "tag");
			BasicBlock* _refValueBlock = BasicBlock::Create(LLVMCONTEXT, "refValue", fun);
			auto boolRefWeight = (boolWeight > refWeight ? boolWeight : refWeight);
			if ((!intsPossible) || (!floatsPossible))
			{
				bool refsMoreLikely = intsPossible ? (boolRefWeight >= intWeight) : (boolRefWeight >= floatWeight);
				MDNode* branchWeights = refsMoreLikely ? GetLikelyFirstBranchMetadata() : GetLikelySecondBranchMetadata();
				auto isRef = builder->CreateICmpEQ(tag, MakeIntLike(tag, 0), "isRef");
				CreateExpect(builder, isRef, MakeIntLike(isRef, refsMoreLikely ? 1 : 0));
				builder->CreateCondBr(isRef, _refValueBlock, intsPossible ? _packedIntBlock : _maskedFloatBlock, branchWeights);
			}
			else
			{
				uint64_t caseweights[3] = { floatWeight, boolRefWeight, intWeight };
				SwitchInst* tagSwitch = builder->CreateSwitch(tag, _maskedFloatBlock, 3, GetBranchWeights(ArrayRef<uint64_t>(caseweights, 3)));
				tagSwitch->addCase(MakeUInt(2, 0), _refValueBlock);
				tagSwitch->addCase(MakeUInt(2, 3), _packedIntBlock);
			}
			if (unpackPrimitives)
			{
				builder->SetInsertPoint(_refValueBlock);
				vtableAsInt = builder->CreatePtrToInt(this->AsPWRef().ReadVTable(builder) , numtype(intptr_t));
				if (intsPossible)
				{
					BasicBlock* unboxIntBlock = BasicBlock::Create(LLVMCONTEXT, "unboxInt", fun);
					builder->SetInsertPoint(_refValueBlock);
					_refValueBlock = BasicBlock::Create(LLVMCONTEXT, "nonIntRefValue", fun);
					auto intAsInt = ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isInt = builder->CreateICmpEQ(vtableAsInt, intAsInt);
					CreateExpect(builder, isInt, MakeIntLike(isInt, intWeight > boolRefWeight && intWeight > floatWeight));
					builder->CreateCondBr(isInt, unboxIntBlock, _refValueBlock, (intWeight > boolRefWeight && intWeight > floatWeight) ? GetLikelyFirstBranchMetadata() : GetLikelySecondBranchMetadata());

					builder->SetInsertPoint(unboxIntBlock);
					auto unboxedInt = builder->CreatePtrToInt(PWObject(val).ReadField(builder, MakeInt32(0), false), INTTYPE, "unboxedInt");
					_primitiveIntPHI->addIncoming(unboxedInt, unboxIntBlock);
					builder->CreateBr(_primitiveIntBlock);
				}
				if (floatsPossible)
				{
					BasicBlock* unboxFloatBlock = BasicBlock::Create(LLVMCONTEXT, "unboxInt", fun);
					builder->SetInsertPoint(_refValueBlock);
					_refValueBlock = BasicBlock::Create(LLVMCONTEXT, "nonIntNonFloatRefValue", fun);
					auto floatAsInt = ConstantExpr::getPtrToInt(NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isFloat = builder->CreateICmpEQ(vtableAsInt, floatAsInt);
					CreateExpect(builder, isFloat, MakeIntLike(isFloat, floatWeight > boolRefWeight));
					builder->CreateCondBr(isFloat, unboxFloatBlock, _refValueBlock, (floatWeight > boolRefWeight) ? GetLikelyFirstBranchMetadata() : GetLikelySecondBranchMetadata());

					builder->SetInsertPoint(unboxFloatBlock);
					auto unboxedFloat = builder->CreateBitCast(builder->CreatePtrToInt(PWObject(val).ReadField(builder, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
					_primitiveFloatPHI->addIncoming(unboxedFloat, unboxFloatBlock);
					builder->CreateBr(_primitiveFloatBlock);
				}
				if (boolsPossible)
				{
					if (primitiveBoolBlock == nullptr || primitiveBoolVar == nullptr)
					{
						throw new std::exception();
					}
					builder->SetInsertPoint(_refValueBlock);
					_refValueBlock = BasicBlock::Create(LLVMCONTEXT, "nonIntNonFloatNonBoolRefValue", fun);
					*primitiveBoolBlock = BasicBlock::Create(LLVMCONTEXT, "boxedBoolean", fun);
					auto boolAsInt = ConstantExpr::getPtrToInt(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isBool = builder->CreateICmpEQ(vtableAsInt, boolAsInt);
					CreateExpect(builder, isBool, MakeIntLike(isBool, boolWeight > refWeight));
					builder->CreateCondBr(isBool, *primitiveBoolBlock, _refValueBlock, (boolWeight > refWeight) ? GetLikelyFirstBranchMetadata() : GetLikelySecondBranchMetadata());

					builder->SetInsertPoint(*primitiveBoolBlock);
					*primitiveBoolVar = UnpackBool(builder, val);
					cases += 1;
				}
			}
			*refValueBlock = _refValueBlock;

			builder->SetInsertPoint(origBlock);

			return cases;
		}
		int NomValue::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight)
		{
			return GenerateRefOrPrimitiveValueSwitch(builder, refValueBlock, intBlock, floatBlock, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, refWeight, intWeight, floatWeight, boolWeight);
		}
	}
}
