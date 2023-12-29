#include "RTRawInt.h"
#include "RTRawFloat.h"
#include "RTRawBool.h"
#include "RTVMPtr.h"
#include "RTObject.h"
#include "RTRefValue.h"
#include "PWAll.h"
#include "RTOutput.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		const RTRawInt* RTRawInt::Get(NomBuilder& builder, const PWInt64 _value, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTRawInt))) RTRawInt(_value, GetIntClassType(), _isfc);
		}
		void RTRawInt::Visit(RTValueVisitor visitor) const
		{
			visitor.VisitRawInt(this);
		}
		const RTPWValuePtr<PWInt64> RTRawInt::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return this;
		}
		const RTPWValuePtr<PWPacked> RTRawInt::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		unsigned int RTRawInt::GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedInt, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, [[maybe_unused]] std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, [[maybe_unused]] uint64_t refWeight, [[maybe_unused]] uint64_t intWeight, [[maybe_unused]] uint64_t floatWeight) const
		{
			onPrimitiveInt(builder, this);
			return 1;
		}
		unsigned int RTRawInt::GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, bool unboxObjects, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight) const
		{
			BasicBlock* newBB = BasicBlock::Create(builder->getContext(), "isPrimitiveInt", builder->GetInsertBlock()->getParent());
			builder->CreateBr(newBB);
			builder->SetInsertPoint(newBB);
			onPrimitiveInt(builder, this);
			return 1;
		}
		PWBool RTRawInt::InstanceOf(NomBuilder& builder, CompileEnv* env, PWType type, bool tryCast, bool optimistic) const
		{
			PWBool ret = type.IsSuperType(builder, env, GetIntClassType(), optimistic);
			if (tryCast)
			{
				BasicBlock* continueBlock = BasicBlock::Create(builder->getContext(), "isInt", builder.GetFunction());
				BasicBlock* errorBlock = RTOutput_Fail::GenerateFailOutputBlock(builder, "Tried to cast integer to incompatible type!");
				builder->CreateCondBr(ret, continueBlock, errorBlock, GetLikelyFirstBranchMetadata());
			}
			else
			{
				return ret;
			}
		}
		PWBool RTRawInt::InstanceOf(NomBuilder& builder, CompileEnv* env, NomTypeRef type, bool tryCast, bool optimistic) const
		{
			return type->IsSupertype(GetIntClassType(), optimistic) ? MakeUInt(1, 1) : MakeUInt(1,0);
		}
		const RTPWValuePtr<PWFloat> RTRawInt::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWFloat, NomTypeRef, RTRawFloat>::Get(builder, builder->CreateUIToFP(value.wrapped, FLOATTYPE, "intToFloat"), GetFloatClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWBool> RTRawInt::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWBool, NomTypeRef, RTRawBool>::Get(builder, builder->CreateICmpNE(value.wrapped, ConstantInt::get(INTTYPE, 0), "intIsNotNull"), GetBoolClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWRefValue> RTRawInt::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWRefValue, NomClassTypeRef, RTRefValue>::Get(builder, PackInt(builder, value.wrapped), static_cast<NomClassTypeRef>(GetIntClassType()), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWVMPtr> RTRawInt::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWVMPtr, NomClassTypeRef, RTVMPtr>::Get(builder, PWVMPtr(builder->CreateIntToPtr(value.wrapped, POINTERTYPE)), static_cast<NomClassTypeRef>(GetIntClassType()), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWObject> RTRawInt::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //technically possible, but stupid; should not happen
		}
		const RTPWValuePtr<PWLambda> RTRawInt::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWPartialApp> RTRawInt::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWRecord> RTRawInt::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWStructVal> RTRawInt::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
	}
}
