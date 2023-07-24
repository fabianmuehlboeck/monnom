#include "PWObject.h"
#include "ObjectHeader.h"
#include "RTConfig.h"
#include "Defs.h"
#include "CompileHelpers.h"
#include "PWType.h"
#include "PWTypeArr.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWObject::GetLLVMType()
		{
			return ObjectHeader::GetLLVMType();
		}
		llvm::Type* PWObject::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		llvm::Value* PWObject::ReadField(NomBuilder &builder, PWInt32 index, bool targetHasRawInvoke)
		{
			return MakeLoad(builder, REFTYPE, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::Fields), ((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? index.Add1(builder) : index)}));
		}
		void PWObject::WriteField(NomBuilder &builder, PWInt32 index, llvm::Value* value, bool targetHasRawInvoke)
		{
			MakeStore(builder, value, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::Fields), ((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? index.Add1(builder) : index) }));
		}
		PWTypeArr PWObject::PointerToTypeArguments(NomBuilder& builder)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::TypeArgs), MakeInt32(0) });
		}
		PWType PWObject::ReadTypeArgument(NomBuilder& builder, PWInt32 index)
		{
			return PWType(MakeInvariantLoad(builder, GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::TypeArgs), index.Neg(builder).Sub1(builder) }));
		}
		void PWObject::WriteTypeArgument(NomBuilder& builder, PWInt32 index, PWType tp)
		{
			MakeInvariantStore(builder, tp, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::TypeArgs), index.Neg(builder).Sub1(builder) }), AtomicOrdering::Unordered);
		}
	}
}
