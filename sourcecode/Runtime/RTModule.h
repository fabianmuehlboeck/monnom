#pragma once

#include "RTClass.h"
#include "NomModule.h"
#include <forward_list>
#include "llvm/IR/Module.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{
		class RTModule
		{
		private:
			std::forward_list<RTClass> classes;
			std::forward_list<RTClassType> classTypes;
			std::unique_ptr<llvm::Module> theModule;
		public:
			//NomJIT::ModuleHandle LLVMModuleHandle;
			RTModule(NomModule * mod);
			~RTModule();
		};


	}
}
