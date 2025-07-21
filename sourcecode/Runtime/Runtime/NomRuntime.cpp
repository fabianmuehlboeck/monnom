#include "../Runtime/NomRuntime.h"
#include "../Runtime/NomJIT.h"
#include "llvm/IR/Module.h"
#include "../Common/Context.h"

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
