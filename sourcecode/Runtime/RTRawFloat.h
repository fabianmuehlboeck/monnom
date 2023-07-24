#pragma once
#include "RTValue.h"
#include "PWFloat.h"

namespace Nom
{
	namespace Runtime
	{
		class RTRawFloat : public RTPWTypedValue<PWFloat, NomClassTypeRef>
		{
		protected:
			RTRawFloat(const PWFloat _value) : RTPWTypedValue<PWFloat, NomClassTypeRef>(_value, static_cast<NomClassTypeRef>(GetFloatClassType()))
			{

			}
		public:
			static const RTRawFloat* Get(NomBuilder& builder, const PWFloat _value);
			virtual ~RTRawFloat() override {}
			virtual NomTypeRef GetType() const override;
			virtual const RTPWValue<PWInt64>* AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWFloat>* AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWBool>* AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWRefValue>* AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValue<PWVMPtr>* AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValue<PWObject>* AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWLambda>* AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWPartialApp>* AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWRecord>* AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValue<PWStructVal>* AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
		};
	}
}
