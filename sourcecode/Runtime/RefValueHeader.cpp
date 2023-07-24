#include "RefValueHeader.h"
#include "Context.h"
#include "RTVTable.h"
#include "Defs.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_os_ostream.h"
POPDIAGSUPPRESSION
#include "CompileHelpers.h"
#include "RTOutput.h"
#include "StringClass.h"
#include <iostream>
#include "RTInterfaceTableEntry.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "CallingConvConf.h"
#include "IMT.h"
#include "RTCompileConfig.h"
#include "BoolClass.h"
#include "RecordHeader.h"
#include "NomClassType.h"
#include "StructuralValueHeader.h"
#include "Metadata.h"
#include "NomMaybeType.h"
#include "PWRefValue.h"
#include "PWPtr.h"
#include "PWObject.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::StructType* RefValueHeader::GetUninitializedLLVMType()
		{
			static llvm::StructType* rvht = llvm::StructType::create(LLVMCONTEXT, "RT_NOM_RefValueHeader");
			return rvht;
		}
		llvm::StructType* RefValueHeader::GetLLVMType()
		{
			static llvm::StructType* rvht = RefValueHeader::GetUninitializedLLVMType();
			static bool once = true;
			if (once)
			{
				once = false;
				rvht->setBody(
					RTVTable::GetLLVMType()->getPointerTo(),	// vtable pointer
					arrtype(POINTERTYPE, 0)						// used for raw invokes with lambda optimziation (for invokable objects, the first field is let free;
																// for structural values, a 64-bit buffer is left between vtable and s-table pointer; in any case
																// accesses to raw invokes should always run through this field)
				);
			}
			return rvht;
		}
		llvm::Value* RefValueHeader::GenerateReadVTablePointer(NomBuilder& builder, llvm::Value* refValue)
		{
			return PWRefValue(refValue).ReadVTable(builder);
		}

		MDNode* GetNominalVsStructuralBranchWeights()
		{
			static MDNode* mdn = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"), ConstantAsMetadata::get(MakeInt32(1)), ConstantAsMetadata::get(MakeInt32(9)) });
			return mdn;
		}

		unsigned int RefValueHeader::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, NomValue value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight)
		{
			return GenerateRefOrPrimitiveValueSwitch(builder, value, refValueBlock, intBlock, floatBlock, false, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, refWeight, intWeight, floatWeight, boolWeight);
		}

		unsigned int RefValueHeader::GenerateNominalStructuralSwitch(NomBuilder& builder, NomValue refValue, llvm::Value** vTableVar, llvm::BasicBlock** nominalObjectBlockVar, llvm::BasicBlock** structuralValueBlockVar)
		{
			if (vTableVar != nullptr && *vTableVar == nullptr)
			{
				*vTableVar = RefValueHeader::GenerateReadVTablePointer(builder, refValue);
			}
			if ((refValue.GetNomType()->GetKind() == TypeKind::TKClass && !((static_cast<NomClassTypeRef>(refValue.GetNomType()))->Named->IsInterface())) || (refValue.GetNomType()->GetKind() == TypeKind::TKMaybe && (static_cast<NomMaybeTypeRef>(refValue.GetNomType()))->PotentialType->GetKind() == TypeKind::TKClass && !((static_cast<NomClassTypeRef>((static_cast<NomMaybeTypeRef>(refValue.GetNomType()))->PotentialType)->Named->IsFunctional()))))
			{
				if (nominalObjectBlockVar == nullptr)
				{
					throw new std::exception();
				}
				if (*nominalObjectBlockVar != nullptr)
				{
					builder->CreateBr(*nominalObjectBlockVar);
				}
				else
				{
					*nominalObjectBlockVar = builder->GetInsertBlock();
				}
				return 1;
			}
			if (refValue.GetNomType()->GetKind() == TypeKind::TKRecord || refValue.GetNomType()->GetKind() == TypeKind::TKLambda || refValue.GetNomType()->GetKind() == TypeKind::TKPartialApp)
			{
				if (structuralValueBlockVar == nullptr)
				{
					throw new std::exception();
				}
				if (*structuralValueBlockVar != nullptr)
				{
					builder->CreateBr(*structuralValueBlockVar);
				}
				else
				{
					*structuralValueBlockVar = builder->GetInsertBlock();
				}
				return 1;
			}
			Value* vtable = nullptr;
			if (vTableVar != nullptr)
			{
				vtable = *vTableVar;
			}
			else
			{
				vtable = RefValueHeader::GenerateReadVTablePointer(builder, refValue);
			}
			Function* fun = builder->GetInsertBlock()->getParent();
			if (nominalObjectBlockVar == nullptr || structuralValueBlockVar == nullptr)
			{
				throw new std::exception();
			}
			if (*nominalObjectBlockVar == nullptr)
			{
				*nominalObjectBlockVar = BasicBlock::Create(LLVMCONTEXT, "nominalObject", builder->GetInsertBlock()->getParent());
			}
			if (*structuralValueBlockVar == nullptr)
			{
				*structuralValueBlockVar = BasicBlock::Create(LLVMCONTEXT, "structuralValue", builder->GetInsertBlock()->getParent());
			}
			auto vTableIsNominal = RTVTable::GenerateIsNominalValue(builder, vtable);
			builder->CreateIntrinsic(llvm::Intrinsic::expect, inttype(1), { vTableIsNominal, MakeUInt(1,1) });
			builder->CreateCondBr(vTableIsNominal, *nominalObjectBlockVar, *structuralValueBlockVar, GetLikelyFirstBranchMetadata());
			return 2;
		}

		unsigned int RefValueHeader::GenerateStructuralValueKindSwitch(NomBuilder& builder, NomValue refValue, llvm::Value** vtableVar, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock)
		{
			auto fun = builder->GetInsertBlock()->getParent();
			if (vtableVar != nullptr && *vtableVar == nullptr)
			{
				*vtableVar = RefValueHeader::GenerateReadVTablePointer(builder, refValue);
			}
			if (lambdaBlock == nullptr || recordBlock == nullptr || partialAppBlock == nullptr)
			{
				throw new std::exception();
			}
			if (refValue.GetNomType()->GetKind() == TypeKind::TKLambda)
			{
				if (*lambdaBlock == nullptr)
				{
					*lambdaBlock = builder->GetInsertBlock();
				}
				else
				{
					builder->CreateBr(*lambdaBlock);
				}
				return 1;
			}
			if (refValue.GetNomType()->GetKind() == TypeKind::TKRecord)
			{
				if (*recordBlock == nullptr)
				{
					*recordBlock = builder->GetInsertBlock();
				}
				else
				{
					builder->CreateBr(*recordBlock);
				}
				return 1;
			}
			if (refValue.GetNomType()->GetKind() == TypeKind::TKPartialApp)
			{
				if (*partialAppBlock == nullptr)
				{
					*partialAppBlock = builder->GetInsertBlock();
				}
				else
				{
					builder->CreateBr(*partialAppBlock);
				}
				return 1;
			}
			if (*lambdaBlock == nullptr)
			{
				*lambdaBlock = BasicBlock::Create(LLVMCONTEXT, "isLambda", fun);
			}
			if (*recordBlock == nullptr)
			{
				*recordBlock = BasicBlock::Create(LLVMCONTEXT, "isRecord", fun);
			}
			if (*partialAppBlock == nullptr)
			{
				*partialAppBlock = BasicBlock::Create(LLVMCONTEXT, "isPartialApp", fun);
			}

			Value* vtable = nullptr;
			if (vtableVar != nullptr)
			{
				vtable = *vtableVar;
			}
			else
			{
				vtable = RefValueHeader::GenerateReadVTablePointer(builder, refValue);
			}
			auto vtableKind = RTVTable::GenerateReadKind(builder, vtable);
			auto switchInst = builder->CreateSwitch(vtableKind, *partialAppBlock, 2, GetBranchWeightsForBlocks({ *partialAppBlock, *lambdaBlock, *recordBlock }));
			switchInst->addCase(MakeIntLike(vtableKind, static_cast<char>(RTDescriptorKind::Lambda)), *lambdaBlock);
			switchInst->addCase(MakeIntLike(vtableKind, static_cast<char>(RTDescriptorKind::Record)), *recordBlock);
			return 3;
		}

		unsigned int RefValueHeader::GenerateRefValueKindSwitch(NomBuilder& builder, NomValue refValue, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock)
		{
			BasicBlock* structuralObjectBlock = nullptr;
			unsigned int ret = GenerateNominalStructuralSwitch(builder, refValue, vtableVar, nominalObjectBlock, &structuralObjectBlock);
			if (structuralObjectBlock != nullptr)
			{
				builder->SetInsertPoint(structuralObjectBlock);
				return ret + GenerateStructuralValueKindSwitch(builder, refValue, vtableVar, lambdaBlock, recordBlock, partialAppBlock) - 1;
			}
			return 1;
		}

		void RefValueHeader::GenerateValueKindSwitch(NomBuilder& builder, NomValue value, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** structuralValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntValVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatValVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolValVar)
		{
			BasicBlock* refValueBlock = nullptr;
			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, intBlock, floatBlock, unpackPrimitives, primitiveIntBlock, primitiveIntValVar, primitiveFloatBlock, primitiveFloatValVar, primitiveBoolBlock, primitiveBoolValVar);

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				RefValueHeader::GenerateNominalStructuralSwitch(builder, value, vtableVar, nominalObjectBlock, structuralValueBlock);
			}
		}
		void RefValueHeader::GenerateValueKindSwitch(NomBuilder& builder, NomValue value, llvm::Value** vtableVar, llvm::BasicBlock** nominalObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** recordBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntValVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatValVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolValVar)
		{
			BasicBlock* refValueBlock = nullptr;
			RefValueHeader::GenerateRefOrPrimitiveValueSwitch(builder, value, &refValueBlock, intBlock, floatBlock, unpackPrimitives, primitiveIntBlock, primitiveIntValVar, primitiveFloatBlock, primitiveFloatValVar, primitiveBoolBlock, primitiveBoolValVar);

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				RefValueHeader::GenerateRefValueKindSwitch(builder, value, vtableVar, nominalObjectBlock, lambdaBlock, recordBlock, partialAppBlock);
			}
		}

		unsigned int RefValueHeader::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, NomValue value, llvm::BasicBlock** refValueBlock, llvm::BasicBlock** intBlock, llvm::BasicBlock** floatBlock, bool unpackPrimitives, llvm::BasicBlock** primitiveIntBlock, llvm::Value** primitiveIntVar, llvm::BasicBlock** primitiveFloatBlock, llvm::Value** primitiveFloatVar, llvm::BasicBlock** primitiveBoolBlock, llvm::Value** primitiveBoolVar, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight)
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			llvm::Type* valType = value->getType();
			if (valType->isIntegerTy(INTTYPE->getIntegerBitWidth()))
			{
				if (primitiveIntBlock == nullptr)
				{
					throw new std::exception();
				}
				*primitiveIntBlock = builder->GetInsertBlock();
				if (primitiveIntVar != nullptr)
				{
					*primitiveIntVar = *value;
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
					*primitiveFloatVar = *value;
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
					*primitiveBoolVar = *value;
				}
				return 1;
			}

			if (unpackPrimitives && value.GetNomType()->IsSubtype(GetIntClassType(), false))
			{
				if (primitiveIntBlock == nullptr || primitiveIntVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveIntVar = UnpackInt(builder, value);
				*primitiveIntBlock = builder->GetInsertBlock();
				return 1;
			}
			else if (unpackPrimitives && value.GetNomType()->IsSubtype(GetFloatClassType(), false))
			{
				if (primitiveFloatBlock == nullptr || primitiveFloatVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveFloatVar = UnpackFloat(builder, value);
				*primitiveFloatBlock = builder->GetInsertBlock();
				return 1;
			}
			else if (unpackPrimitives && value.GetNomType()->IsSubtype(GetBoolClassType(), false))
			{
				if (primitiveBoolBlock == nullptr || primitiveBoolVar == nullptr)
				{
					throw new std::exception();
				}
				*primitiveBoolVar = UnpackBool(builder, value);
				*primitiveBoolBlock = builder->GetInsertBlock();
				return 1;
			}

			bool intsPossible = !value.GetNomType()->IsDisjoint(GetIntClassType());
			bool floatsPossible = !value.GetNomType()->IsDisjoint(GetFloatClassType());
			bool boolsPossible = !value.GetNomType()->IsDisjoint(GetFloatClassType());

			unsigned int cases = 1; //could always be normal reference at this point

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
					auto unpackedInt = UnpackMaskedInt(builder, builder->CreatePtrToInt(value, INTTYPE, "refAsInt"));
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
					auto unpackedFloat = UnpackMaskedFloat(builder, builder->CreatePtrToInt(value, INTTYPE, "refAsInt"));
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
					vtableAsInt = builder->CreatePtrToInt(RefValueHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
					auto boolAsInt = ConstantExpr::getPtrToInt(NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isBool = builder->CreateICmpEQ(vtableAsInt, boolAsInt);
					builder->CreateCondBr(isBool, *primitiveBoolBlock, *refValueBlock);
					builder->SetInsertPoint(*primitiveBoolBlock);
					*primitiveBoolVar = UnpackBool(builder, value);
					return 2;
				}
			}
			builder->SetInsertPoint(origBlock);
			auto refAsInt = builder->CreatePtrToInt(value, INTTYPE, "refAsInt");
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
				vtableAsInt = builder->CreatePtrToInt(RefValueHeader::GenerateReadVTablePointer(builder, value), numtype(intptr_t));
				if (intsPossible)
				{
					BasicBlock* unboxIntBlock = BasicBlock::Create(LLVMCONTEXT, "unboxInt", fun);
					builder->SetInsertPoint(_refValueBlock);
					_refValueBlock = BasicBlock::Create(LLVMCONTEXT, "nonIntRefValue", fun);
					auto intAsInt = ConstantExpr::getPtrToInt(NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()), numtype(intptr_t));
					auto isInt = builder->CreateICmpEQ(vtableAsInt, intAsInt);
					CreateExpect(builder, isInt, MakeIntLike(isInt, intWeight > boolRefWeight && intWeight > floatWeight));
					builder->CreateCondBr(isInt, unboxIntBlock, _refValueBlock, (intWeight > boolRefWeight && intWeight > floatWeight)?GetLikelyFirstBranchMetadata():GetLikelySecondBranchMetadata());

					builder->SetInsertPoint(unboxIntBlock);
					auto unboxedInt = builder->CreatePtrToInt(PWObject(value).ReadField(builder, MakeInt32(0), false), INTTYPE, "unboxedInt");
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
					auto unboxedFloat = builder->CreateBitCast(builder->CreatePtrToInt(PWObject(value).ReadField(builder, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
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
					*primitiveBoolVar = UnpackBool(builder, value);
					cases += 1;
				}
			}
			*refValueBlock = _refValueBlock;

			builder->SetInsertPoint(origBlock);

			return cases;
		}
		llvm::Value* RefValueHeader::GenerateReadTypeTag(NomBuilder& builder, llvm::Value* refValue)
		{
			return PWRefValue(refValue).ReadTypeTag(builder);
		}
		llvm::Value* RefValueHeader::GenerateGetReserveTypeArgsFromVTablePointer(NomBuilder& builder, llvm::Value* vtablePtr)
		{
			auto pointerAsInt = builder->CreatePtrToInt(vtablePtr, numtype(intptr_t));
			return builder->CreateTrunc(builder->CreateLShr(pointerAsInt, MakeIntLike(pointerAsInt, bitsin(intptr_t) / 2)), numtype(int32_t));
		}

		void RefValueHeader::GenerateWriteVTablePointer(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr)
		{
			PWRefValue(refValue).WriteVTable(builder, vtableptr);
		}
		llvm::Value* RefValueHeader::GenerateWriteVTablePointerCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtableptr, llvm::Value* vtableValue)
		{
			return PWRefValue(refValue).WriteVTableCMPXCHG(builder, vtableptr, vtableValue);
		}
		void RefValueHeader::GenerateInitializerCode(NomBuilder& builder, llvm::Value* valueHeader, llvm::Constant* vTablePointer, llvm::Constant* rawInvokePointer)
		{
			GenerateWriteVTablePointer(builder, valueHeader, vTablePointer);
			if (rawInvokePointer != nullptr)
			{
				GenerateWriteRawInvoke(builder, valueHeader, rawInvokePointer);
			}
		}

		llvm::Value* RefValueHeader::GetInterfaceMethodTableFunction(NomBuilder& builder, CompileEnv* env, RegIndex reg, llvm::Constant* index, [[maybe_unused]] size_t lineno)
		{
			Function* fun = builder->GetInsertBlock()->getParent();
			auto recreg = (*env)[reg];

			BasicBlock* refValueBlock = nullptr, * packedIntBlock = nullptr, * packedFloatBlock = nullptr, * primitiveIntBlock = nullptr, * primitiveFloatBlock = nullptr, * primitiveBoolBlock = nullptr;

			BasicBlock* mergeBlock = nullptr;
			PHINode* tablePHI = nullptr;

			GenerateRefOrPrimitiveValueSwitch(builder, recreg, &refValueBlock, &packedIntBlock, &packedFloatBlock, false, &primitiveIntBlock, nullptr, &primitiveFloatBlock, nullptr, &primitiveBoolBlock, nullptr);

			unsigned int count = (refValueBlock != nullptr ? 1 : 0) + (packedIntBlock != nullptr ? 1 : 0) + (packedFloatBlock != nullptr ? 1 : 0) + (primitiveIntBlock != nullptr ? 1 : 0) + (primitiveFloatBlock != nullptr ? 1 : 0) + (primitiveBoolBlock != nullptr ? 1 : 0);

			if (count == 0)
			{
				throw new std::exception();
			}
			if (count > 1)
			{
				mergeBlock = BasicBlock::Create(LLVMCONTEXT, "imtLookupMerge", fun);

				builder->SetInsertPoint(mergeBlock);
				tablePHI = builder->CreatePHI(GetIMTFunctionType()->getPointerTo(), count, "imt");
			}

			if (refValueBlock != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				Value* vtable = GenerateReadVTablePointer(builder, recreg);
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, vtable, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (packedIntBlock != nullptr)
			{
				builder->SetInsertPoint(packedIntBlock);
				auto clsConstant = NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveIntBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveIntBlock);
				auto clsConstant = NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (packedFloatBlock != nullptr)
			{
				builder->SetInsertPoint(packedFloatBlock);
				auto clsConstant = NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveFloatBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveFloatBlock);
				auto clsConstant = NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			if (primitiveBoolBlock != nullptr)
			{
				builder->SetInsertPoint(primitiveBoolBlock);
				auto clsConstant = NomBoolClass::GetInstance()->GetLLVMElement(*fun->getParent());
				auto vte = RTVTable::GenerateReadInterfaceMethodTableEntry(builder, clsConstant, index);
				if (count == 1)
				{
					return vte;
				}
				else
				{
					tablePHI->addIncoming(vte, builder->GetInsertBlock());
					builder->CreateBr(mergeBlock);
				}
			}
			builder->SetInsertPoint(mergeBlock);
			return tablePHI;
		}

		llvm::Value* RefValueHeader::GenerateReadRawInvoke(NomBuilder& builder, llvm::Value* refValue)
		{
			return PWRefValue(refValue).ReadRawInvoke(builder);
		}


		void RefValueHeader::GenerateWriteRawInvoke(NomBuilder& builder, llvm::Value* refValue, llvm::Value* rawInvokePointer)
		{
			PWRefValue(refValue).WriteRawInvoke(builder, rawInvokePointer);
		}
		llvm::AtomicCmpXchgInst* RefValueHeader::GenerateWriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* refValue, llvm::Value* previousValueForCMPXCHG, llvm::Value* rawInvokePointer)
		{
			return PWRefValue(refValue).WriteRawInvokeCMPXCHG(builder, rawInvokePointer, previousValueForCMPXCHG);
		}
	}
}
