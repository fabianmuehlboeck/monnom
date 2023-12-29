#include "PWDispatchPair.h"
#include "CompileHelpers.h"
#include "IMT.h"

using namespace std;
using namespace llvm;

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWDispatchPair::GetLLVMType()
		{
			return GetDynamicDispatcherLookupResultType();
		}
		llvm::Type* PWDispatchPair::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWDispatchPair PWDispatchPair::Get(NomBuilder & builder, PWIMTFunction funptr, llvm::Value* receiver)
		{
			auto partialPair = builder->CreateInsertValue(UndefValue::get(GetDynamicDispatcherLookupResultType()), funptr, { 0 });
			return builder->CreateInsertValue(partialPair, receiver, { 1 });
		}
		PWIMTFunction PWDispatchPair::GetFunction(NomBuilder& builder) const
		{
			return builder->CreateExtractValue(wrapped, { 0 }, "dispatchFunction");
		}
		llvm::Value* PWDispatchPair::GetReceiver(NomBuilder& builder) const
		{
			return builder->CreateExtractValue(wrapped, { 1 }, "dispatchReceiver");
		}
	}
}
