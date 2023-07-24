#include "RTVMPtr.h"
#include "RTRawInt.h"
#include "RTRawBool.h"
#include "RTRawFloat.h"

namespace Nom
{
	namespace Runtime
	{
		RTVMPtr::RTVMPtr(PWVMPtr _value, NomTypeRef _type) : RTPWTypedValue<PWVMPtr, NomTypeRef>(_value, _type)
		{

		}
		const RTVMPtr* RTVMPtr::Get(NomBuilder& builder, PWVMPtr _value, NomTypeRef _type)
		{
			return new(builder.Malloc(sizeof(RTVMPtr))) RTVMPtr(_value, _type);
		}
		const RTPWValue<PWInt64>* RTVMPtr::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetIntClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWInt64, NomClassTypeRef, RTRawInt>::Get(builder, builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), static_cast<NomClassTypeRef>(GetIntClassType()), orig==nullptr?this:orig);
		}
		const RTPWValue<PWFloat>* RTVMPtr::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetFloatClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWFloat, NomClassTypeRef, RTRawFloat>::Get(builder, builder->CreateBitCast(builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), FLOATTYPE, "intBitToFloat"), static_cast<NomClassTypeRef>(GetFloatClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWBool>* RTVMPtr::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			if (check && !type->IsSubtype(GetBoolClassType(), false))
			{
				throw std::exception();
			}
			return RTConvValue<PWBool, NomClassTypeRef, RTRawBool>::Get(builder, builder->CreateTrunc(builder->CreatePtrToInt(value.wrapped, INTTYPE, "ptrToInt"), BOOLTYPE, "intToBool"), static_cast<NomClassTypeRef>(GetBoolClassType()), orig == nullptr ? this : orig);
		}
		const RTPWValue<PWRefValue>* RTVMPtr::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return RTConvValue<PWRefValue, NomTypeRef, RTRefValue>::Get(builder, value.wrapped, type, orig==nullptr?this:orig);
		}
		const RTPWValue<PWVMPtr>* RTVMPtr::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return this;
		}
		const RTPWValue<PWObject>* RTVMPtr::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWObject, NomTypeRef, RTObject>::Get(builder, value.wrapped, type, orig == nullptr ? this : orig);
		}
		const RTPWValue<PWLambda>* RTVMPtr::AsLambda(NomBuilder & builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWLambda, NomTypeRef, RTLambdaValue>::Get(builder, value.wrapped, type, orig == nullptr ? this : orig);
		}
		const RTPWValue<PWPartialApp>* RTVMPtr::AsPartialApp(NomBuilder & builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWPartialApp, NomTypeRef, RTPartialAppValue>::Get(builder, value.wrapped, type, orig == nullptr ? this : orig);
		}
		const RTPWValue<PWRecord>* RTVMPtr::AsRecord(NomBuilder & builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWRecord, NomTypeRef, RTRecordValue>::Get(builder, value.wrapped, type, orig == nullptr ? this : orig);
		}
		const RTPWValue<PWStructVal>* RTVMPtr::AsStructVal(NomBuilder & builder, RTValuePtr orig, bool check) const
		{
			return RTConvValue<PWStructVal, NomTypeRef, RTStructuralValue>::Get(builder, value.wrapped, type, orig == nullptr ? this : orig);
		}
	}
}
