#include "RTLambdaValue.h"
#include "PWLambda.h"

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		RTLambdaValue::RTLambdaValue(const PWLambda _val, NomTypeRef _type, bool _isfc) : RTPWTypedValue<PWLambda, NomTypeRef>(_val, _type, _isfc)
		{

		}
		RTLambdaValue* RTLambdaValue::Get(NomBuilder& builder, const PWLambda _val, NomTypeRef _type, bool _isfc)
		{
			return new(builder.Malloc(sizeof(RTLambdaValue))) RTLambdaValue(_val, _type, _isfc);
		}
		const RTPWValuePtr<PWInt64> RTLambdaValue::AsRawInt(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWFloat> RTLambdaValue::AsRawFloat(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWBool> RTLambdaValue::AsRawBool(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRefValue> RTLambdaValue::AsRefValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWVMPtr> RTLambdaValue::AsVMPtr(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWObject> RTLambdaValue::AsObject(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWLambda> RTLambdaValue::AsLambda(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPartialApp> RTLambdaValue::AsPartialApp(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWRecord> RTLambdaValue::AsRecord(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWStructVal> RTLambdaValue::AsStructVal(NomBuilder& builder, RTValuePtr orig, bool check) const
		{
			return nullptr;
		}
		const RTPWValuePtr<PWPacked> RTLambdaValue::AsPackedValue(NomBuilder& builder, RTValuePtr orig) const
		{
			return nullptr;
		}
		int RTLambdaValue::GenerateRefOrPrimitiveValueSwitchUnpackPrimitives(NomBuilder& builder, std::function<void(NomBuilder&, RTPWValuePtr<PWRefValue>)> onRefValue, std::function<void(NomBuilder&, RTPWValuePtr<PWInt64>)> onPrimitiveInt, std::function<void(NomBuilder&, RTPWValuePtr<PWFloat>)> onPrimitiveFloat, std::function<void(NomBuilder&, RTPWValuePtr<PWBool>)> onPrimitiveBool, bool unboxObjects, uint64_t refWeight, uint64_t intWeight, uint64_t floatWeight, uint64_t boolWeight) const
		{
			BasicBlock* newBB = BasicBlock::Create(builder->getContext(), "isLambda", builder->GetInsertBlock()->getParent());
			builder->CreateBr(newBB);
			builder->SetInsertPoint(newBB);
			onRefValue(builder, this->AsRefValue(builder));
			return 1;
		}
	}
}
