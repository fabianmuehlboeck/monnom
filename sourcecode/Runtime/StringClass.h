#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "NomVMInterface.h"

//namespace Nom
//{
//	namespace Runtime
//	{
//		class RTStringClass : public RTClass
//		{
//		public:
//			RTStringClass();
//		};
//
//	}
//}

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

//extern const Nom::Runtime::RTStringClass _RTStringClass;
//extern const Nom::Runtime::NomStringClass _NomStringClass;
//extern const Nom::Runtime::NomStringClass * const _NomStringClassRef;
//extern const Nom::Runtime::RTStringClass * const _RTStringClassRef;
//extern const Nom::Runtime::NomClass * const _NomStringClassNC;
//extern const Nom::Runtime::RTClass * const _RTStringClassRTC;

extern "C" DLLEXPORT void * LIB_NOM_String_Print_1(void* str);
extern "C" DLLEXPORT void* foo(void* str);
llvm::Function* GetDebugPrint(llvm::Module* mod);

llvm::Function* GetDebugPrintHex(llvm::Module* mod);
