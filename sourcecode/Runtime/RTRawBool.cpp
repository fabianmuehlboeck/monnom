#include "RTRawBool.h"
#include "RTRawInt.h"
#include "RTRawFloat.h"
#include "RTVMPtr.h"
#include "RTObject.h"
#include "RTRefValue.h"
#include "PWRefValue.h"
#include "PWObject.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		const RTRawBool* RTRawBool::Get(NomBuilder& builder, const PWBool _value, bool _isfc = false)
		{
			return new(builder.Malloc(sizeof(RTRawBool))) RTRawBool(_value, GetBoolClassType(), _isfc);
		}
		NomTypeRef RTRawBool::GetNomType() const
		{
			return GetBoolClassType();
		}
		const RTPWValuePtr<PWInt64> RTRawBool::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWInt64, NomTypeRef, RTRawInt>::Get(builder, value.Resize<64>(builder), GetIntClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWFloat> RTRawBool::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWFloat, NomTypeRef, RTRawFloat>::Get(builder, builder->CreateUIToFP(value.wrapped, FLOATTYPE, "boolToFloat"), GetFloatClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWBool> RTRawBool::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return this;
		}
		const RTPWValuePtr<PWRefValue> RTRawBool::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWRefValue, NomTypeRef, RTRefValue>::Get(builder, PWRefValue(PackBool(builder, value.wrapped)), GetBoolClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWVMPtr> RTRawBool::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWVMPtr, NomTypeRef, RTVMPtr>::Get(builder, PWVMPtr(builder->CreateIntToPtr(value.Resize<64>(builder), POINTERTYPE)), GetBoolClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWObject> RTRawBool::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWObject, NomTypeRef, RTObject>::Get(builder, PWObject(PackBool(builder, value.wrapped)), GetBoolClassType(), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWLambda> RTRawBool::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWPartialApp> RTRawBool::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWRecord> RTRawBool::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWStructVal> RTRawBool::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValuePtr<PWPacked> RTRawBool::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		int RTRawBool::GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, bool unboxObjects, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight) const
		{
			BasicBlock* newBB = BasicBlock::Create(builder->getContext(), "isPrimitiveBool", builder->GetInsertBlock()->getParent());
			builder->CreateBr(newBB);
			builder->SetInsertPoint(newBB);
			onPrimitiveBool(builder, this);
			return 1;
		}
	}
}
