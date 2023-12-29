#pragma once
#include "RTValue.h"
#include "PWLambda.h"

namespace Nom
{
	namespace Runtime
	{
		class RTLambdaValue : public RTPWTypedValue<PWLambda, NomTypeRef>
		{
		protected:
			RTLambdaValue(const PWLambda _val, NomTypeRef _type, bool _isfc = false);
		public:
			static RTLambdaValue* Get(NomBuilder& builder, const PWLambda _val, NomTypeRef _type, bool _isfc = false);
			virtual ~RTLambdaValue() override {}
			virtual void Visit(RTValueVisitor visitor) const override;
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual unsigned int GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, bool unboxObjects, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight) const override;
		};
	}
}
