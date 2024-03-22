#include "PWIMTFunction.h"
#include "IMT.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::Type* PWIMTFunction::GetWrappedLLVMType()
		{
			return GetIMTFunctionType();
		}
	}
}
