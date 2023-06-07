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
		llvm::Value* PWTypeVar::ReadIndex(NomBuilder& builder)
		{
			return MakeLoad(builder, GetLLVMType(), wrapped, MakeInt32(RTTypeVarFields::Index), "typeVarIndex");
		}
	}
}