#include "NomRuntime.h"
#include "NomJIT.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Module.h"
POPDIAGSUPPRESSION
#include "Context.h"

using namespace llvm;

namespace Nom
{
	namespace Runtime
	{
		void NomRuntime::Run()
		{
			std::unique_ptr<Module> TheModule = std::make_unique<Module>("Nom Runtime Main", LLVMCONTEXT);
		}
	}
}
