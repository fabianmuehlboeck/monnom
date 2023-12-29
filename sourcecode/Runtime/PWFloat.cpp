#include "PWFloat.h"
#include "CompileHelpers.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWFloat::GetLLVMType()
		{
			return FLOATTYPE;
		}
		llvm::Type* PWFloat::GetWrappedLLVMType()
		{
			return FLOATTYPE;
		}
	}
}
