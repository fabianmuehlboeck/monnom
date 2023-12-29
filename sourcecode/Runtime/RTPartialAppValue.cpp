#include "RTPartialAppValue.h"
#include "PWAll.h"

namespace Nom
{
	namespace Runtime
	{
		RTPartialAppValue::RTPartialAppValue(const PWPartialApp _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWPartialApp, NomTypeRef>(_val, _type, _isfc)
		{
		}
		void RTPartialAppValue::Visit(RTValueVisitor visitor) const
		{
			visitor.VisitPartialApp(this);
		}
		RTPartialAppValue* RTPartialAppValue::Get(NomBuilder& builder, const PWPartialApp _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTPartialAppValue))) RTPartialAppValue(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTPartialAppValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWFloat> RTPartialAppValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWBool> RTPartialAppValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRefValue> RTPartialAppValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPacked> RTPartialAppValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWVMPtr> RTPartialAppValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWObject> RTPartialAppValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWLambda> RTPartialAppValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPartialApp> RTPartialAppValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRecord> RTPartialAppValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWStructVal> RTPartialAppValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
	}
}
