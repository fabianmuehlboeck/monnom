#include "RTStructuralValue.h"
#include "PWAll.h"

namespace Nom
{
	namespace Runtime
	{
		RTStructuralValue::RTStructuralValue(const PWStructVal _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWStructVal, NomTypeRef>(_val, _type, _isfc)
		{
		}
		RTStructuralValue* RTStructuralValue::Get(NomBuilder& builder, const PWStructVal _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTStructuralValue))) RTStructuralValue(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTStructuralValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWInt64>();
		}
		const RTPWValuePtr<PWFloat> RTStructuralValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWFloat>();
		}
		const RTPWValuePtr<PWBool> RTStructuralValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWBool>();
		}
		const RTPWValuePtr<PWRefValue> RTStructuralValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTPWValuePtr<PWRefValue>();
		}
		const RTPWValuePtr<PWPacked> RTStructuralValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTPWValuePtr<PWPacked>();
		}
		const RTPWValuePtr<PWVMPtr> RTStructuralValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTPWValuePtr<PWVMPtr>();
		}
		const RTPWValuePtr<PWObject> RTStructuralValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWObject>();
		}
		const RTPWValuePtr<PWLambda> RTStructuralValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWLambda>();
		}
		const RTPWValuePtr<PWPartialApp> RTStructuralValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWPartialApp>();
		}
		const RTPWValuePtr<PWRecord> RTStructuralValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWRecord>();
		}
		const RTPWValuePtr<PWStructVal> RTStructuralValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTPWValuePtr<PWStructVal>();
		}
		void RTStructuralValue::Visit(RTValueVisitor visitor) const
		{
			visitor.VisitStructValue(this);
		}
	}
}
