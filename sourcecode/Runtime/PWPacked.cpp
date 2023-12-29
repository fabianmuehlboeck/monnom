#include "PWPacked.h"
#include "CompileHelpers.h"
#include "RefValueHeader.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/DerivedTypes.h"
POPDIAGSUPPRESSION

using namespace llvm;
namespace Nom
{
	namespace Runtime
	{

		llvm::Value* PWPacked::ReadTypeTag(NomBuilder& builder) const
		{
			return builder->CreateTrunc(builder->CreatePtrToInt(MakeLoad(builder, 
				RefValueHeader::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable)), numtype(intptr_t)), IntegerType::get(LLVMCONTEXT, 3));
		}
		llvm::Type* PWPacked::GetLLVMType()
		{
			return RefValueHeader::GetLLVMType();
		}
		llvm::Type* PWPacked::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
	}
}
