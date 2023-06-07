#include "PWTypeVar.h"
#include "CompileHelpers.h"
#include "RTClassType.h"
#include "PWClassType.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWClassType::GetLLVMType()
		{
			return RTClassType::GetLLVMType();
		}
	}
}