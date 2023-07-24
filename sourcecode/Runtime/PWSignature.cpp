#include "PWSignature.h"
#include "RTSignature.h"
#include "PWType.h"
#include "PWVMPtr.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWSignature::GetLLVMType()
		{
			return RTSignature::GetLLVMType();
		}
		llvm::Type* PWSignature::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWType PWSignature::ReadReturnType(NomBuilder& builder) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTSignatureFields::ReturnType), "returnType");
		}
		PWInt32 PWSignature::ReadTypeParamCount(NomBuilder& builder) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTSignatureFields::TypeParamCount), "typeParamCount");
		}
		PWInt32 PWSignature::ReadParamCount(NomBuilder& builder) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTSignatureFields::ParamCount), "paramCount");
		}
		PWVMPtr PWSignature::ReadLLVMFunctionType(NomBuilder& builder) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTSignatureFields::LLVMFunctionType), "llvmFunctionType");
		}
		PWType PWSignature::ReadTypeParameter(NomBuilder& builder, PWInt32 index) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, { MakeInt32(RTSignatureFields::TypeParameters), builder->CreateSub(MakeInt32(-1), index) }, "typeParam");
		}
		PWType PWSignature::ReadParameter(NomBuilder& builder, PWInt32 index) const
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, { MakeInt32(RTSignatureFields::ParameterTypes), index }, "paramType");
		}
	}
}
