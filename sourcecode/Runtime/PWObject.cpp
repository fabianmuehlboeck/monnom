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
		llvm::Value* PWObject::ReadField(NomBuilder &builder, int index, bool targetHasRawInvoke)
		{
			return MakeLoad(builder, REFTYPE, builder->CreateGEP(GetLLVMType(), wrapped, {MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::Fields), MakeInt<int32_t>((index + ((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 1 : 0)))}));
		}
		llvm::Value* PWObject::ReadField(NomBuilder &builder, llvm::Value* index, bool targetHasRawInvoke)
		{
			return MakeLoad(builder, REFTYPE, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::Fields), builder->CreateAdd(index, MakeInt32(((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 1 : 0))) }));
		}
		void PWObject::WriteField(NomBuilder &builder, int index, llvm::Value *value, bool targetHasRawInvoke)
		{
			MakeStore(builder, value, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::Fields), MakeInt<int32_t>((index + ((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 1 : 0))) }));
		}
		void PWObject::WriteField(NomBuilder &builder, llvm::Value* index, llvm::Value* value, bool targetHasRawInvoke)
		{
			MakeStore(builder, value, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::Fields), builder->CreateAdd(index, MakeInt32(((targetHasRawInvoke && (NomLambdaOptimizationLevel > 0)) ? 1 : 0))) }));
		}
		PWTypeArr PWObject::PointerToTypeArguments(NomBuilder& builder)
		{
			return builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt32(0), MakeInt32(ObjectHeaderFields::TypeArgs), MakeInt32(0) });
		}
		PWType PWObject::ReadTypeArgument(NomBuilder& builder, int32_t index)
		{
			return PWType(MakeInvariantLoad(builder, GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::TypeArgs), MakeInt<int32_t>((-1) - index) }));
		}
		PWType PWObject::ReadTypeArgument(NomBuilder& builder, llvm::Value* index)
		{
			return PWType(MakeInvariantLoad(builder, GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::TypeArgs), builder->CreateSub(MakeInt<int32_t>(-1),index) }));
		}
		void PWObject::WriteTypeArgument(NomBuilder& builder, int32_t index, PWType tp)
		{
			MakeInvariantStore(builder, tp, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::TypeArgs), MakeInt<int32_t>((-1) - index) }), AtomicOrdering::Unordered);
		}
		void PWObject::WriteTypeArgument(NomBuilder& builder, llvm::Value* index, PWType tp)
		{
			MakeInvariantStore(builder, tp, builder->CreateGEP(GetLLVMType(), wrapped, { MakeInt<int32_t>(0), MakeInt<int32_t>((unsigned char)ObjectHeaderFields::TypeArgs), builder->CreateSub(MakeInt<int32_t>(-1), index) }), AtomicOrdering::Unordered);
		}
	}
}
