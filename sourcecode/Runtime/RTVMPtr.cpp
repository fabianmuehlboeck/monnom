#include "RTVMPtr.h"
#include "RTRawInt.h"
#include "RTRawBool.h"
#include "RTRawFloat.h"
#include "RTRefValue.h"
#include "RTObject.h"
#include "RTPartialAppValue.h"
#include "RTLambdaValue.h"
#include "RTRecordValue.h"
#include "RTStructuralValue.h"
#include "PWAll.h"

namespace Nom
{
	namespace Runtime
	{
		RTVMPtr::RTVMPtr(PWVMPtr _value, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWVMPtr, NomTypeRef>(_value, _type, _isfc)
		{

		}
		const RTVMPtr* RTVMPtr::Get(NomBuilder& builder, PWVMPtr _value, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTVMPtr))) RTVMPtr(_value, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTVMPtr::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetIntClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWInt64, NomClassTypeRef, RTRawInt>::Get(builder, builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), static_cast<NomClassTypeRef>(GetIntClassType()), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWFloat> RTVMPtr::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetFloatClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWFloat, NomClassTypeRef, RTRawFloat>::Get(builder, builder->CreateBitCast(builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), FLOATTYPE, "intBitToFloat"), static_cast<NomClassTypeRef>(GetFloatClassType()), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWBool> RTVMPtr::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetBoolClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWBool, NomClassTypeRef, RTRawBool>::Get(builder, builder->CreateTrunc(builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), BOOLTYPE, "intToBool"), static_cast<NomClassTypeRef>(GetBoolClassType()), orig.Coalesce(this));
		}
		const RTPWValuePtr<PWRefValue> RTVMPtr::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWRefValue, NomTypeRef, RTRefValue>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWVMPtr> RTVMPtr::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return this;
		}
		const RTPWValuePtr<PWObject> RTVMPtr::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWObject, NomTypeRef, RTObject>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWLambda> RTVMPtr::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWLambda, NomTypeRef, RTLambdaValue>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWPartialApp> RTVMPtr::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWPartialApp, NomTypeRef, RTPartialAppValue>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWRecord> RTVMPtr::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWRecord, NomTypeRef, RTRecordValue>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWStructVal> RTVMPtr::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWStructVal, NomTypeRef, RTStructuralValue>::Get(builder, value.wrapped, type, orig.Coalesce(this));
		}
		const RTPWValuePtr<PWPacked> RTVMPtr::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		void RTVMPtr::Visit(RTValueVisitor visitor) const
		{
			visitor.VisitVMPtr(this);
		}
	}
}
