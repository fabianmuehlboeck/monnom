#pragma once
#include "RTValue.h"
#include "PWObject.h"

namespace Nom
{
	namespace Runtime
	{
		class RTObject : public RTPWTypedValue<PWObject, NomTypeRef>
		{
		protected:
			RTObject(const PWObject _val, NomTypeRef _type, bool _isfc=false);
		public:
			virtual ~RTObject() override {}
			static RTObject* Get(NomBuilder& builder, const PWObject _val, NomTypeRef _type, bool _isfc = false);
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check = false) const override;
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig) const override;
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig) const override;
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const override;
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig) const override;
			virtual void Visit(RTValueVisitor visitor) const override;
		};
	}
}
