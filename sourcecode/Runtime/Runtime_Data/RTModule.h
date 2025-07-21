#pragma once

#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomModule.h"
#include <forward_list>
#include "llvm/IR/Module.h"
#include "../Runtime/NomJIT.h"
#include "../Runtime/NomTypeRegistry.h"

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
			static std::forward_list<void*> &structRecords();
			static std::forward_list<void*> &lambdaRecords();
			static std::forward_list<RuntimeInstantiationDictionary*> &instantiationDictionaries();
		public:
			static void ClearCaches();
			RTModule(NomModule * mod);
			~RTModule();
		};


	}
}
