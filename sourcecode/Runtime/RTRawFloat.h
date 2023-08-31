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
			RTRawFloat(const PWFloat _value, [[maybe_unused]] NomTypeRef _type, bool _isfc = false) : RTPWTypedValue<PWFloat, NomClassTypeRef>(_value, static_cast<NomClassTypeRef>(GetFloatClassType()), _isfc)
			{

			}
		public:
			static const RTRawFloat* Get(NomBuilder& builder, const PWFloat _value, bool _isfc = false);
			virtual ~RTRawFloat() override {}
			virtual NomTypeRef GetNomType() const override;
			virtual const RTPWValuePtr<PWInt64> AsRawInt(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWFloat> AsRawFloat(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWBool> AsRawBool(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWRefValue> AsRefValue(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWVMPtr> AsVMPtr(NomBuilder& builder, RTValuePtr orig = nullptr) const override;
			virtual const RTPWValuePtr<PWObject> AsObject(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWLambda> AsLambda(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWPartialApp> AsPartialApp(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWRecord> AsRecord(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWStructVal> AsStructVal(NomBuilder& builder, RTValuePtr orig = nullptr, bool check) const override;
			virtual const RTPWValuePtr<PWPacked> AsPackedValue(NomBuilder& builder, RTValuePtr orig) const override;
			virtual int GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt = nullptr, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat = nullptr, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool = nullptr, bool unboxObjects = false, uint64_t refWeight = 100, uint64_t intWeight = 50, uint64_t floatWeight = 40, uint64_t boolWeight = 30) const override;
		};
	}
}
