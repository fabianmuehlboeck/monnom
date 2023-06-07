#include "PWRefValue.h"
#include "Defs.h"
#include "CompileHelpers.h"

using namespace std;
using namespace llvm;
namespace Nom
{
	namespace Runtime
	{
		llvm::Value* Nom::Runtime::PWRefValue::ReadTypeTag(NomBuilder& builder) const
		{
			return builder->CreateTrunc(builder->CreatePtrToInt(MakeLoad(builder, RefValueHeader::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable)), numtype(intptr_t)), IntegerType::get(LLVMCONTEXT, 3));
		}
		PWVTable PWRefValue::ReadVTable(NomBuilder& builder) const
		{
			return PWVTable(MakeInvariantLoad(builder, RefValueHeader::GetLLVMType(), wrapped, MakeInt32(RefValueHeaderFields::InterfaceTable)));
		}
	}
}