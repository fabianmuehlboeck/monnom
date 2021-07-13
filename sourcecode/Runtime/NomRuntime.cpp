#include "NomRuntime.h"
#include "NomJIT.h"
#include "llvm/IR/Module.h"
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
