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
			RTVMPtr(PWVMPtr _value, NomTypeRef _type, bool _isfc = false);
		public:
			static const RTVMPtr* Get(NomBuilder& builder, PWVMPtr _value, NomTypeRef _type, bool _isfc = false);
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const override;
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
			void Visit(RTValueVisitor visitor) const override;
		};
	}
}
