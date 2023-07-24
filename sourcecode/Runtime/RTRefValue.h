#pragma once
#include "RTValue.h"

namespace Nom
{
	namespace Runtime
	{
		class RTRefValue : public RTPWTypedValue<PWRefValue, NomTypeRef>
		{
		protected:
			RTRefValue(PWRefValue _value, NomTypeRef _type);
		public:
			static const RTRefValue* Get(NomBuilder& builder, PWRefValue _value, NomTypeRef _type);
			virtual const RTPWValue<PWInt64>* AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWFloat>* AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWBool>* AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWRefValue>* AsRefValue(NomBuilder& builder, RTValuePtr orig) const override;
			virtual const RTPWValue<PWVMPtr>* AsVMPtr(NomBuilder& builder, RTValuePtr orig) const override;
			virtual const RTPWValue<PWObject>* AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWLambda>* AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWPartialApp>* AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWRecord>* AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValue<PWStructVal>* AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const override;
		};
	}
}
