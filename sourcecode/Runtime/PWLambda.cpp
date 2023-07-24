#include "PWLambda.h"
#include "LambdaHeader.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Value* PWLambda::ReadField(NomBuilder& builder, size_t fieldIndex)
		{
			return MakeInvariantLoad(builder, REFTYPE, builder->CreateGEP(LambdaHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldIndex) }));
		}
		llvm::Value* PWLambda::WriteField(NomBuilder& builder, size_t fieldIndex, llvm::Value* value)
		{
			return MakeInvariantStore(builder, value, builder->CreateGEP(LambdaHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldIndex) }));
		}
		llvm::Value* PWLambdaPrecise::ReadLambdaField(NomBuilder& builder, size_t fieldIndex)
		{
			return MakeInvariantLoad(builder, REFTYPE, builder->CreateGEP(LambdaHeader::GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(LambdaHeaderFields::Fields), MakeInt32(fieldIndex) }));
		}
	}
}
