#pragma once
#include "RTValue.h"

namespace Nom
{
	namespace Runtime
	{
		class RTPackedValue : public RTPWTypedValue<PWPacked, NomTypeRef>
		{
		protected:
			RTPackedValue(const PWPacked _val, NomTypeRef _type, bool _isfc=false);
		public:
			virtual ~RTPackedValue() override {}
			static RTPackedValue* Get(NomBuilder& builder, const PWPacked _val, NomTypeRef _type, bool _isfc = false);
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check = false) const override;
			virtual int GenerateRefOrPrimitiveValueSwitch(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedInt = nullptr, std::function<void(NomBuilder&, RTPWValuePtr<PWPacked>)> onPackedFloat = nullptr, uint64_t refWeight = 100, uint64_t intWeight = 50, uint64_t floatWeight = 30) const override;
			virtual int GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt = nullptr, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat = nullptr, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool = nullptr, bool unboxObjects = false, uint64_t refWeight = 100, uint64_t intWeight = 50, uint64_t floatWeight = 30, uint64_t boolWeight = 10) const override;
		};
	}
}
