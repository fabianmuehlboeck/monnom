#pragma once
#include "RTValue.h"
#include "PWInt.h"
#include "NomClassType.h"

namespace Nom
{
	namespace Runtime
	{
		class RTRawInt : public RTPWTypedValue<PWInt64, NomClassTypeRef>
		{
		protected:
			RTRawInt(const PWInt64 _value) : RTPWTypedValue<PWInt64, NomClassTypeRef>(_value, static_cast<NomClassTypeRef>(GetIntClassType()))
			{

			}
		public:
			static const RTRawInt* Get(NomBuilder& builder, const PWInt64 _value);
			virtual const RTPWValue<PWInt64>* AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWFloat>* AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWBool>* AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWRefValue>* AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValue<PWVMPtr>* AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValue<PWObject>* AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWLambda>* AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWPartialApp>* AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWRecord>* AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValue<PWStructVal>* AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
		};
	}
}
