#include "RTRawBool.h"
#include "RTRawInt.h"
#include "RTRawFloat.h"
#include "RTVMPtr.h"
#include "RTObject.h"
#include "RTRefValue.h"

namespace Nom
{
	namespace Runtime
	{
		const RTRawBool& RTRawBool::Get(NomBuilder& builder, const PWBool _value)
		{
			return *new(builder.Malloc(sizeof(RTRawBool))) RTRawBool(_value);
		}
		NomTypeRef RTRawBool::GetType() const
		{
			return GetBoolClassType();
		}
		const RTPWValue<PWInt64>* RTRawBool::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWInt64, NomTypeRef, RTRawInt>::Get(builder, value.Resize<64>(builder), GetIntClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWFloat>* RTRawBool::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWFloat, NomTypeRef, RTRawFloat>::Get(builder, builder->CreateUIToFP(value.wrapped, FLOATTYPE, "boolToFloat"), GetFloatClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWBool>* RTRawBool::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return this;
		}
		const RTPWValue<PWRefValue>* RTRawBool::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWRefValue, NomTypeRef, RTRefValue>::Get(builder, PWRefValue(PackBool(builder, value.wrapped)), GetBoolClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWVMPtr>* RTRawBool::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWVMPtr, NomTypeRef, RTVMPtr>::Get(builder, PWVMPtr(builder->CreateIntToPtr(value.Resize<64>(builder), POINTERTYPE)), GetBoolClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWObject>* RTRawBool::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWObject, NomTypeRef, RTObject>::Get(builder, PWObject(PackBool(builder, value.wrapped)), GetBoolClassType(), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWLambda>* RTRawBool::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWPartialApp>* RTRawBool::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWRecord>* RTRawBool::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
		const RTPWValue<PWStructVal>* RTRawBool::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception();
		}
	}
}
