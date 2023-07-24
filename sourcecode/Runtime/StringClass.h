#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "NomVMInterface.h"

extern int NomDebugPrintLevel;
namespace Nom
{
	namespace Runtime
	{
		class ObjectHeader;
		class NomStringClass : public NomClassInternal
		{
		private:
			NomStringClass();
		public:
			static NomStringClass *GetInstance();
			virtual ~NomStringClass() override {}
		};

		
	}
}

extern "C" DLLEXPORT void LIB_NOM_DEBUG_PRINT(void* str, int64_t index, decltype(NomDebugPrintLevel) level);
extern "C" DLLEXPORT void LIB_NOM_DEBUG_PRINT_HEX(void* str, int64_t index, decltype(NomDebugPrintLevel) level);
extern "C" DLLEXPORT void * LIB_NOM_String_Print_1(void* str);
llvm::Function* GetDebugPrint(llvm::Module* mod);

llvm::Function* GetDebugPrintHex(llvm::Module* mod);
