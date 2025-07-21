#include "../Common/Context.h"
#include "../Runtime/NomJIT.h"

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