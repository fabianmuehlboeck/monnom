#include "RTRecordValue.h"
#include "PWRecord.h"

namespace Nom
{
	namespace Runtime
	{
		RTRecordValue::RTRecordValue(const PWRecord _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWRecord, NomTypeRef>(_val, _type, _isfc)
		{
		}
		RTRecordValue* RTRecordValue::Get(NomBuilder& builder, const PWRecord _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTRecordValue))) RTRecordValue(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTRecordValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWFloat> RTRecordValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWBool> RTRecordValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRefValue> RTRecordValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPacked> RTRecordValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWVMPtr> RTRecordValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWObject> RTRecordValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWLambda> RTRecordValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPartialApp> RTRecordValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRecord> RTRecordValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWStructVal> RTRecordValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
	}
}
