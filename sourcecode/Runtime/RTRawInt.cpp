#pragma once
#include "RTRawInt.h"
#include "RTRawFloat.h"
#include "RTRawBool.h"
#include "RTVMPtr.h"
#include "RTObject.h"
#include "RTRefValue.h"

namespace Nom
{
	namespace Runtime
	{
		const RTRawInt* RTRawInt::Get(NomBuilder& builder, const PWInt64 _value)
		{
			return new(builder.Malloc(sizeof(RTRawInt))) RTRawInt(_value);
		}
		const RTPWValue<PWInt64>* RTRawInt::AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return this;
		}
		const RTPWValue<PWFloat>* RTRawInt::AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return RTConvValue<PWFloat, NomTypeRef, RTRawFloat>::Get(builder, builder->CreateUIToFP(value.wrapped, FLOATTYPE, "intToFloat"), GetFloatClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWBool>* RTRawInt::AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			return RTConvValue<PWBool, NomTypeRef, RTRawBool>::Get(builder, builder->CreateICmpNE(value.wrapped, ConstantInt::get(INTTYPE, 0), "intIsNotNull"), GetBoolClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWRefValue>* RTRawInt::AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const
		{
			return RTConvValue<PWRefValue, NomClassTypeRef, RTRefValue>::Get(builder, PackInt(builder, value.wrapped), static_cast<NomClassTypeRef>(GetIntClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWVMPtr>* RTRawInt::AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const
		{
			return RTConvValue<PWVMPtr, NomClassTypeRef, RTVMPtr>::Get(builder, PWVMPtr(builder->CreateIntToPtr(value.wrapped, POINTERTYPE)), static_cast<NomClassTypeRef>(GetIntClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWObject>* RTRawInt::AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception(); //technically possible, but stupid; should not happen
		}
		const RTPWValue<PWLambda>* RTRawInt::AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWPartialApp>* RTRawInt::AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWRecord>* RTRawInt::AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWStructVal>* RTRawInt::AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const
		{
			throw new std::exception();
		}
	}
}
