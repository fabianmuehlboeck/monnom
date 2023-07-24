#include "RTRefValue.h"

namespace Nom
{
	namespace Runtime
	{
		RTRefValue::RTRefValue(PWRefValue _value, NomTypeRef _type) : RTPWTypedValue<PWRefValue, NomTypeRef>(_value, _type)
		{

		}
		const RTRefValue* RTRefValue::Get(NomBuilder& builder, PWRefValue _value, NomTypeRef _type)
		{
			return new(builder.Malloc(sizeof(RTRefValue))) RTRefValue(_value, _type);
		}
		const RTPWValue<PWInt64>* RTRefValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValue<PWFloat>* RTRefValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValue<PWBool>* RTRefValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValue<PWRefValue>* RTRefValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return this;
		}
		const RTPWValue<PWVMPtr>* RTRefValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValue<PWObject>* RTRefValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValue<PWLambda>* RTRefValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValue<PWPartialApp>* RTRefValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValue<PWRecord>* RTRefValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValue<PWStructVal>* RTRefValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
	}
}
