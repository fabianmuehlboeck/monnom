#pragma once
#include "RTValue.h"

namespace Nom
{
	namespace Runtime
	{
		class RTRawBool : public RTPWValue<PWBool>
		{
		protected:
			RTRawBool(const PWBool _value) : RTPWValue<PWBool>(_value) {}
		public:
			static const RTRawBool& Get(NomBuilder& builder, const PWBool _value);
			virtual NomTypeRef GetType() const override;
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
