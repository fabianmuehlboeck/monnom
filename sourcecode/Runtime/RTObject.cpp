#include "RTObject.h"
#include "PWObject.h"

namespace Nom
{
	namespace Runtime
	{
		RTObject::RTObject(const PWObject _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue(_val, _type, _isfc)
		{
		}
		RTObject* RTObject::Get(NomBuilder& builder, const PWObject _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTObject))) RTObject(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTObject::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWFloat> RTObject::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWBool> RTObject::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRefValue> RTObject::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWVMPtr> RTObject::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWObject> RTObject::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWLambda> RTObject::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPartialApp> RTObject::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRecord> RTObject::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWStructVal> RTObject::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPacked> RTObject::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
	}
}
