#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "RTTypeVar.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWTypeVar::GetLLVMType()
		{
			return RTTypeVar::GetLLVMType();
		}
		llvm::Type* PWTypeVar::GetWrappedLLVMType()
		{
			return NLLVMPointer(GetLLVMType());
		}
		PWInt32 PWTypeVar::ReadIndex(NomBuilder& builder)
		{
			return PWInt64(MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTTypeVarFields::Index), "typeVarIndex")).Resize<32>(builder);
		}
	}
}
