#pragma once
#include "RTValue.h"
#include "PWVMPtr.h"

namespace Nom
{
	namespace Runtime
	{
		class RTVMPtr : public RTPWTypedValue<PWVMPtr, NomTypeRef>
		{
		protected:
			RTVMPtr(PWVMPtr _value, NomTypeRef _type);
		public:
			static const RTVMPtr* Get(NomBuilder& builder, PWVMPtr _value, NomTypeRef _type);
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
