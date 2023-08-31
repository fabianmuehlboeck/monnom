#include "RTPackedValue.h"
#include "PWPacked.h"
#include "RTOutput.h"
#include "PWPhi.h"
#include "RTRawFloat.h"
#include "RTRawInt.h"
#include "RTRawBool.h"
#include "BoolClass.h"
#include "PWRefValue.h"
#include "PWVTable.h"
#include "IntClass.h"
#include "FloatClass.h"
#include "PWObject.h"
#include "RTRefValue.h"
#include "RTVMPtr.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		RTPackedValue::RTPackedValue(const PWPacked _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWPacked, NomTypeRef>(_val, _type, _isfc)
		{
		}
		RTPackedValue* RTPackedValue::Get(NomBuilder& builder, const PWPacked _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTPackedValue))) RTPackedValue(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTPackedValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			BasicBlock* returnBlock = BasicBlock::Create(builder->getContext(), "ensuredIntBlock", builder->GetInsertBlock()->getParent());
			if (this->GetNomType()->IsDisjoint(GetIntClassType()))
			{
				throw new std::exception();
			}
			RTPWValuePtr<PWInt64> ret = nullptr;
			GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
				[](NomBuilder& b, RTPWValuePtr<PWRefValue> v) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Encountered unexpected reference value", b->GetInsertBlock());
				},
				[&ret, returnBlock](NomBuilder& b, RTPWValuePtr<PWInt64> v) -> void {
					ret = v;
					b->CreateBr(returnBlock);
				},
				nullptr,
				nullptr,
				true,
				10,
				100,
				10,
				10);
			builder->SetInsertPoint(returnBlock);
			return RTConvValue<PWInt64, NomTypeRef, RTRawInt>::Get(builder, ret->value, GetIntClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWFloat> RTPackedValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			BasicBlock* returnBlock = BasicBlock::Create(builder->getContext(), "ensuredFloatBlock", builder->GetInsertBlock()->getParent());
			if (this->GetNomType()->IsDisjoint(GetFloatClassType()))
			{
				throw new std::exception();
			}
			RTPWValuePtr<PWFloat> ret = nullptr;
			GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
				[](NomBuilder& b, RTPWValuePtr<PWRefValue> v) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Encountered unexpected reference value", b->GetInsertBlock());
				},
				nullptr,
				[&ret, returnBlock](NomBuilder& b, RTPWValuePtr<PWFloat> v) -> void {
					ret = v;
					b->CreateBr(returnBlock);
				},
				nullptr,
				true,
				10,
				10,
				100,
				10);
			builder->SetInsertPoint(returnBlock);
			return RTConvValue < PWFloat, NomTypeRef, RTRawFloat > ::Get(builder, ret->value, GetFloatClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWBool> RTPackedValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			BasicBlock* returnBlock = BasicBlock::Create(builder->getContext(), "ensuredBoolBlock", builder->GetInsertBlock()->getParent());
			if (this->GetNomType()->IsDisjoint(GetBoolClassType()))
			{
				throw new std::exception();
			}
			RTPWValuePtr<PWBool> ret = nullptr;
			GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(builder,
				[](NomBuilder& b, RTPWValuePtr<PWRefValue> v) -> void {
					RTOutput_Fail::MakeBlockFailOutputBlock(b, "Encountered unexpected reference value", b->GetInsertBlock());
				},
				nullptr,
				nullptr,
				[&ret, returnBlock](NomBuilder& b, RTPWValuePtr<PWBool> v) -> void {
					ret = v;
					b->CreateBr(returnBlock);
				},
				true,
				10,
				10,
				10,
				100);
			builder->SetInsertPoint(returnBlock);
			return RTConvValue < PWBool, NomTypeRef, RTRawBool > ::Get(builder, ret->value, GetBoolClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWRefValue> RTPackedValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			BasicBlock* returnBlock = BasicBlock::Create(builder->getContext(), "ensuredRefBlock", builder->GetInsertBlock()->getParent());
			RTPWValuePtr<PWRefValue> ret = nullptr;
			GenerateRefOrPrimitiveValueSwitch(builder,
				[&ret, returnBlock](NomBuilder& b, RTPWValuePtr<PWRefValue> v) -> void {
					ret = v;
					b->CreateBr(returnBlock);
				});
			builder->SetInsertPoint(returnBlock);
			return RTConvValue < PWRefValue, NomTypeRef, RTRefValue > ::Get(builder, ret->value, GetNomType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWPacked> RTPackedValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return this;
		}
		const RTPWValuePtr<PWVMPtr> RTPackedValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue < PWVMPtr, NomTypeRef, RTVMPtr > ::Get(builder, value.wrapped, GetNomType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWObject> RTPackedValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return AsRefValue(builder, orig)->AsObject(builder, orig, check);
		}
		const RTPWValuePtr<PWLambda> RTPackedValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return AsRefValue(builder, orig)->AsLambda(builder, orig, check);
		}
		const RTPWValuePtr<PWPartialApp> RTPackedValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return AsRefValue(builder, orig)->AsPartialApp(builder, orig, check);
		}
		const RTPWValuePtr<PWRecord> RTPackedValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return AsRefValue(builder, orig)->AsRecord(builder, orig, check);
		}
		const RTPWValuePtr<PWStructVal> RTPackedValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return AsRefValue(builder, orig)->AsStructVal(builder, orig, check);
		}
		int RTPackedValue::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedInt, std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedFloat, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight) const
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			bool couldBeInt = !GetIntClassType()->IsDisjoint(type);
			bool couldBeFloat = !GetFloatClassType()->IsDisjoint(type);
			if (!(couldBeInt || couldBeFloat))
			{
				//BasicBlock* newBB = BasicBlock::Create(builder->getContext(), "isRefValue", fun);
				//builder->CreateBr(newBB);
				//builder->SetInsertPoint(newBB);
				onRefValue(builder, this->AsRefValue(builder));
				return 1;
			}
			if (!couldBeInt)
			{
				intWeight = 0;
			}
			if (!couldBeFloat)
			{
				floatWeight = 0;
			}

			BasicBlock* refValueBlock = BasicBlock::Create(builder->getContext(), "isRefValue", fun);
			BasicBlock* packedIntBlock = BasicBlock::Create(builder->getContext(), "isPackedInt", fun);
			BasicBlock* packedFloatBlock = BasicBlock::Create(builder->getContext(), "isPackedFloat", fun);

			PWPacked pwp = this->value;
			auto typeTag = builder->CreateTrunc(pwp.ReadTypeTag(builder), inttype(2));
			uint64_t caseweights[4] = { refWeight, floatWeight, floatWeight, intWeight /*, 0, floatWeight, floatWeight, intWeight*/ };
			auto tagSwitch = builder->CreateSwitch(typeTag, refValueBlock, 4, GetBranchWeights(caseweights));
			tagSwitch->addCase(MakeIntLike(typeTag, 0), refValueBlock);
			tagSwitch->addCase(MakeIntLike(typeTag, 1), packedFloatBlock);
			tagSwitch->addCase(MakeIntLike(typeTag, 2), packedFloatBlock);
			tagSwitch->addCase(MakeIntLike(typeTag, 3), packedIntBlock);
			//tagSwitch->addCase(MakeIntLike(typeTag, 4), refValueBlock);
			//tagSwitch->addCase(MakeIntLike(typeTag, 5), packedFloatBlock);
			//tagSwitch->addCase(MakeIntLike(typeTag, 6), packedFloatBlock);
			//tagSwitch->addCase(MakeIntLike(typeTag, 7), packedIntBlock);
			int cases = 0;
			if (onRefValue.target<decltype(onRefValue)>() != nullptr)
			{
				builder->SetInsertPoint(refValueBlock);
				onRefValue(builder, this->AsRefValue(builder));
				cases++;
			}
			else
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Encountered unexpected reference value", refValueBlock);
			}

			if (onPackedInt.target<decltype(onPackedInt)>() != nullptr)
			{
				builder->SetInsertPoint(packedIntBlock);
				onPackedInt(builder, this->AsPackedValue(builder));
				cases++;
			}
			else
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Encountered unexpected packed integer", packedIntBlock);
			}

			if (onPackedFloat.target<decltype(onPackedFloat)>() != nullptr)
			{
				builder->SetInsertPoint(packedIntBlock);
				onPackedFloat(builder, this->AsPackedValue(builder));
				cases++;
			}
			else
			{
				RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Encountered unexpected packed float", packedFloatBlock);
			}

			return cases;
		}
		int RTPackedValue::GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, bool unboxObjects, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight) const
		{
			BasicBlock* origBlock = builder->GetInsertBlock();
			Function* fun = origBlock->getParent();
			bool pIntFunPresent = onPrimitiveInt.target<decltype(onPrimitiveInt)>() != nullptr;
			bool pFloatFunPresent = onPrimitiveFloat.target<decltype(onPrimitiveFloat)>() != nullptr;
			bool pBoolFunPresent = onPrimitiveBool.target<decltype(onPrimitiveBool)>() != nullptr;
			bool couldBeInt = (!GetIntClassType()->IsDisjoint(type)) && pIntFunPresent;
			bool couldBeFloat = (!GetFloatClassType()->IsDisjoint(type)) && pFloatFunPresent;
			bool couldBeBool = (!GetBoolClassType()->IsDisjoint(type)) && pBoolFunPresent;
			if ((!unboxObjects) || !(couldBeInt || couldBeFloat || couldBeBool))
			{
				return GenerateRefOrPrimitiveValueSwitch(builder, onRefValue,
					pIntFunPresent ?
					[onPrimitiveInt](NomBuilder& _b, RTPWValuePtr<PWPacked> _iv) -> void
					{
						onPrimitiveInt(_b, _iv->AsRawInt(_b));
					} : static_cast<std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)>>(nullptr),
						pFloatFunPresent ?
						[onPrimitiveFloat](NomBuilder& _b, RTPWValuePtr<PWPacked> _fv) -> void
					{
					} : static_cast<std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)>>(nullptr),
						refWeight, intWeight, floatWeight);
			}
			BasicBlock* intBlock = nullptr;
			BasicBlock* floatBlock = nullptr;
			RTPWValuePtr<PWInt64> rawInt;
			RTPWValuePtr<PWFloat> rawFloat;
			PWPhi<PWInt64> intPHI = nullptr;
			PWPhi<PWFloat> floatPHI = nullptr;
			if (!GetIntClassType()->IsDisjoint(type))
			{
				intBlock = BasicBlock::Create(builder->getContext(), "handlePrimitiveInt", fun);
				builder->SetInsertPoint(intBlock);
				if (pIntFunPresent)
				{
					intPHI = PWPhi<PWInt64>::Create(builder, 2, "primitiveInt");
					onPrimitiveInt(builder, RTRawInt::Get(builder, intPHI, false));
				}
				else
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Encountered unexpected packed int", intBlock);
				}
			}
			if (!GetFloatClassType()->IsDisjoint(type))
			{
				floatBlock = BasicBlock::Create(builder->getContext(), "handlePrimitiveFloat", fun);
				builder->SetInsertPoint(floatBlock);
				if (pFloatFunPresent)
				{
					floatPHI = PWPhi<PWFloat>::Create(builder, 2, "primitiveFloat");
					onPrimitiveFloat(builder, RTRawFloat::Get(builder, floatPHI, false));
				}
				else
				{
					RTOutput_Fail::MakeBlockFailOutputBlock(builder, "Encountered unexpected packed float", floatBlock);
				}
			}
			GenerateRefOrPrimitiveValueSwitch(builder,
				[onRefValue, onPrimitiveInt, onPrimitiveFloat, onPrimitiveBool, unboxObjects, couldBeInt, couldBeFloat, couldBeBool, refWeight, intWeight, floatWeight, boolWeight, this, intBlock, floatBlock, intPHI, floatPHI](NomBuilder& b, RTPWValuePtr<PWRefValue> _rv) -> void
				{
					BasicBlock* origBlock = b->GetInsertBlock();
					Function* fun = origBlock->getParent();
					if (couldBeBool)
					{
						if (GetNomType()->IsSubtype(GetBoolClassType(), false))
						{
							auto boolVal = NomBoolObjects::BoolObjToRawBool(b, value);
							onPrimitiveBool(b, RTRawBool::Get(b, boolVal, false));
							return;
						}
						BasicBlock* boolBlock = BasicBlock::Create(b->getContext(), "isBoolBlock", fun);
						BasicBlock* notBoolBlock = BasicBlock::Create(b->getContext(), "notBoolBlock", fun);
						auto isTrue = CreatePointerEq(b, _rv, NomBoolObjects::GetTrue(*fun->getParent()), "isTrue");
						auto isFalse = CreatePointerEq(b, _rv, NomBoolObjects::GetFalse(*fun->getParent()), "isTrue");
						auto isBool = b->CreateOr(isTrue, isFalse, "isBool");
						b->CreateCondBr(isBool, boolBlock, notBoolBlock, ((refWeight + intWeight + floatWeight) > boolWeight) ? GetLikelySecondBranchMetadata() : GetLikelyFirstBranchMetadata());

						b->SetInsertPoint(boolBlock);
						auto boolVal = NomBoolObjects::BoolObjToRawBool(b, value);
						onPrimitiveBool(b, RTRawBool::Get(b, boolVal, false));

						b->SetInsertPoint(notBoolBlock);
					}
					auto thisAsRef = this->AsRefValue(b);
					PWVTable pwvt = nullptr;
					if (couldBeInt || couldBeFloat)
					{
						if (couldBeInt)
						{
							if (GetNomType()->IsSubtype(GetIntClassType(), false))
							{
								if (!intPHI.IsEmpty())
								{
									auto rawInt = b->CreatePtrToInt(PWObject(value).ReadField(b, MakeInt32(0), false), INTTYPE, "unboxedInt");
									intPHI->addIncoming(rawInt, b->GetInsertBlock());
								}
								b->CreateBr(intBlock);
								return;
							}
							pwvt = thisAsRef->value.ReadVTable(b);
							BasicBlock* isIntBlock = BasicBlock::Create(b->getContext(), "isIntBlock", fun);
							BasicBlock* notIntBlock = BasicBlock::Create(b->getContext(), "notIntBlock", fun);
							auto isInt = pwvt.CompareWith(b, NomIntClass::GetInstance()->GetLLVMElement(*fun->getParent()));
							b->CreateCondBr(isInt, isIntBlock, notIntBlock, ((refWeight + floatWeight) > intWeight) ? GetLikelySecondBranchMetadata() : GetLikelyFirstBranchMetadata());

							b->SetInsertPoint(isIntBlock);
							if (!intPHI.IsEmpty())
							{
								auto rawInt = b->CreatePtrToInt(PWObject(value).ReadField(b, MakeInt32(0), false), INTTYPE, "unboxedInt");
								intPHI->addIncoming(rawInt, b->GetInsertBlock());
							}
							b->CreateBr(intBlock);

							b->SetInsertPoint(notIntBlock);
						}
						if (couldBeFloat)
						{
							if (GetNomType()->IsSubtype(GetFloatClassType(), false))
							{
								if (!floatPHI.IsEmpty())
								{
									auto rawFloat = b->CreateBitCast(b->CreatePtrToInt(PWObject(value).ReadField(b, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
									floatPHI->addIncoming(rawFloat, b->GetInsertBlock());
								}
								b->CreateBr(floatBlock);
								return;
							}
							if (pwvt.IsEmpty())
							{
								pwvt = thisAsRef->value.ReadVTable(b);
							}
							BasicBlock* isFloatBlock = BasicBlock::Create(b->getContext(), "isFloatBlock", fun);
							BasicBlock* notFloatBlock = BasicBlock::Create(b->getContext(), "notFloatBlock", fun);
							auto isFloat = pwvt.CompareWith(b, NomFloatClass::GetInstance()->GetLLVMElement(*fun->getParent()));
							b->CreateCondBr(isFloat, isFloatBlock, notFloatBlock, (refWeight > floatWeight) ? GetLikelySecondBranchMetadata() : GetLikelyFirstBranchMetadata());

							b->SetInsertPoint(isFloatBlock);
							if (!floatPHI.IsEmpty())
							{
								auto rawFloat = b->CreateBitCast(b->CreatePtrToInt(PWObject(value).ReadField(b, MakeInt32(0), false), INTTYPE), FLOATTYPE, "unboxedFloat");
								floatPHI->addIncoming(rawFloat, b->GetInsertBlock());
							}
							b->CreateBr(floatBlock);

							b->SetInsertPoint(notFloatBlock);
						}
					}
					onRefValue(b, thisAsRef);
				},
				[intPHI, intBlock](NomBuilder& b, RTPWValuePtr<PWPacked> _iv) -> void
				{
					if (!intPHI.IsEmpty())
					{
						auto rawInt = _iv->AsRawInt(b);
						intPHI->addIncoming(rawInt, b->GetInsertBlock());
					}
					b->CreateBr(intBlock);
				},
				[floatPHI, floatBlock](NomBuilder& b, RTPWValuePtr<PWPacked> _fv) -> void
				{
					if (!floatPHI.IsEmpty())
					{
						auto rawFloat = _fv->AsRawFloat(b);
						floatPHI->addIncoming(rawFloat, b->GetInsertBlock());
					}
					b->CreateBr(floatBlock);
				},
				refWeight, intWeight, floatWeight);
		}
	}
}
