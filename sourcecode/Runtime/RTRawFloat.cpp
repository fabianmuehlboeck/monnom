#include "RTRawFloat.h"
#include "RTRawInt.h"
#include "RTRawBool.h"
#include "RTVMPtr.h"
#include "RTObject.h"
#include "RTRefValue.h"

namespace Nom
{
	namespace Runtime
	{
		const RTRawFloat* RTRawFloat::Get(NomBuilder& builder, const PWFloat _value)
		{
			return new(builder.Malloc(sizeof(RTRawFloat))) RTRawFloat(_value);
		}
		NomTypeRef RTRawFloat::GetType() const
		{
			return GetFloatClassType();
		}
		const RTPWValue<PWInt64>* RTRawFloat::AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return RTConvValue < PWInt64, NomClassTypeRef, RTRawInt > ::Get(builder, builder->CreateFPToSI(value.wrapped, INTTYPE, "floatToInt"), static_cast<NomClassTypeRef>(GetIntClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWFloat>* RTRawFloat::AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return this;
		}
		const RTPWValue<PWBool>* RTRawFloat::AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return RTConvValue<PWBool, NomClassTypeRef, RTRawBool>::Get(builder, builder->CreateFCmpUNE(value.wrapped, ConstantFP::get(FLOATTYPE, 0.0), "floatIsNotNull"), static_cast<NomClassTypeRef>(GetBoolClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWRefValue>* RTRawFloat::AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const
		{
			return RTConvValue<PWRefValue, NomClassTypeRef, RTRefValue>::Get(builder, PackFloat(builder, value.wrapped), static_cast<NomClassTypeRef>(GetFloatClassType()), orig==nullptr?this:orig);
		}
		const RTPWValue<PWVMPtr>* RTRawFloat::AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const
		{
			return RTConvValue<PWVMPtr, NomClassTypeRef, RTVMPtr>::Get(builder, PWVMPtr(builder->CreateIntToPtr(builder->CreateBitCast(value.wrapped, INTTYPE), POINTERTYPE)), static_cast<NomClassTypeRef>(GetFloatClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWObject>* RTRawFloat::AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception(); //technically possible, but stupid; should not happen
		}
		const RTPWValue<PWLambda>* RTRawFloat::AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWPartialApp>* RTRawFloat::AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWRecord>* RTRawFloat::AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWStructVal>* RTRawFloat::AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
	}
}
