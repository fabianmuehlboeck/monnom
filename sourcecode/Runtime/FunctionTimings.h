#pragma once
#include "NomVMInterface.h"

extern "C" DLLEXPORT void NOM_RT_FT_EnterFunction(size_t funid);
extern "C" DLLEXPORT void NOM_RT_FT_LeaveFunction();

namespace Nom
{
	namespace Runtime
	{
		llvm::Function* GetEnterFunctionFunction(llvm::Module& mod);
		llvm::Function* GetLeaveFunctionFunction(llvm::Module& mod);

		void InitFunctionTimings();

		void PrintFunctionTimings();
	}
}
