#include "RTRefValue.h"
#include "PWAll.h"

namespace Nom
{
	namespace Runtime
	{
		RTRefValue::RTRefValue(PWRefValue _value, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWRefValue, NomTypeRef>(_value, _type, _isfc)
		{

		}
		const RTRefValue* RTRefValue::Get(NomBuilder& builder, PWRefValue _value, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTRefValue))) RTRefValue(_value, _type, _isfc);
		}
		void RTRefValue::Visit(RTValueVisitor visitor) const
		{
			visitor.VisitRefValue(this);
		}
		const RTPWValuePtr<PWInt64> RTRefValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValuePtr<PWFloat> RTRefValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValuePtr<PWBool> RTRefValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			throw new std::exception(); //should be resolved earlier
		}
		const RTPWValuePtr<PWRefValue> RTRefValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return this;
		}
		const RTPWValuePtr<PWVMPtr> RTRefValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWObject> RTRefValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWLambda> RTRefValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPartialApp> RTRefValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRecord> RTRefValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWStructVal> RTRefValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPacked> RTRefValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
	}
}
