#include "Context.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::LLVMContext &TheContext()
		{
			return NomJIT::Instance().getContext();
		}
	}
}
