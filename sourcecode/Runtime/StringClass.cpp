#include "StringClass.h"
#include "ObjectClass.h"
#include "NomAlloc.h"
#include <iostream>
#include "Defs.h"
#include "NomMethod.h"
#include "NullClass.h"
#include "NomConstants.h"
#include "ObjectHeader.h"
#include "VoidClass.h"
#include "NomJIT.h"
#include "CompileHelpers.h"
#include "NomMethodTableEntry.h"
#include "RTConfig.h"
#include "llvm/Support/DynamicLibrary.h"
#include "IntClass.h"
#include "CastStats.h"
#include "NomClassType.h"
#include "Runtime.h"

using namespace Nom::Runtime;

//Nom::Runtime::NomStringClass *_NomStringClass= Nom::Runtime::NomStringClass::GetInstance();
//const Nom::Runtime::RTStringClass _RTStringClass; 
//const Nom::Runtime::NomStringClass * const _NomStringClassRef = &_NomStringClass;
//const Nom::Runtime::RTStringClass * const _RTStringClassRef = &_RTStringClass;
//const Nom::Runtime::NomClass * const _NomStringClassNC = &_NomStringClass;
//const Nom::Runtime::RTClass * const _RTStringClassRTC = &_RTStringClass;

//inline Nom::Runtime::RTStringClass::RTStringClass() : RTClass(0, 0)
//{
//	SuperTypes.push_back(&_RTObjectClassType);
//	methodtable = (void **) nalloc(sizeof(void *) * 1);
//	methodtable[0] = (void *)LIB_NOM_String_Print_1;
//}
static int64_t debugPrint_lastIndices[4][100];
static std::string* debugPrint_lastStrs[4][100];
static decltype(NomDebugPrintLevel) debugPrint_lastLevel = 0;
static int debugPrint_rbufpos = 0;

static int64_t debugPrintX_lastIndices[4][100];
static std::string* debugPrintX_lastStrs[4][100];
static decltype(NomDebugPrintLevel) debugPrintX_lastLevel = 0;
static int debugPrintX_rbufpos = 0;

extern "C" DLLEXPORT void LIB_NOM_DEBUG_PRINT(void* str, int64_t index, decltype(NomDebugPrintLevel) level)
{
	if (level <= 3)
	{
		debugPrint_lastLevel = level;
		debugPrint_lastIndices[level][debugPrint_rbufpos] = index;
		debugPrint_lastStrs[level][debugPrint_rbufpos] = (std::string*)str;
		debugPrint_rbufpos = (debugPrint_rbufpos + 1) % 100;
	}
	if (NomDebugPrintLevel >= level)
	{
		std::cout << (std::string*)str;
		std::cout << ":";
		std::cout << std::dec << index;
		std::cout << "\n";
		std::cout.flush();
	}
	//if (NomStatsLevel > 1)
	//{
	//	RT_NOM_STATS_IncProfileCounter((std::string*) str);
	//}
	return;
}
extern "C" DLLEXPORT void LIB_NOM_DEBUG_PRINT_HEX(void* str, int64_t index, decltype(NomDebugPrintLevel) level)
{
	if (level <= 3)
	{
		debugPrintX_lastLevel = level;
		debugPrintX_lastIndices[level][debugPrintX_rbufpos] = index;
		debugPrintX_lastStrs[level][debugPrintX_rbufpos] = (std::string*)str;
		debugPrintX_rbufpos = (debugPrintX_rbufpos + 1) % 100;
	}
	if (NomDebugPrintLevel >= level)
	{
		std::cout << (std::string*)str;
		std::cout << ":";
		std::cout << std::hex << index;
		std::cout << "\n";
		std::cout.flush();
	}
	return;
}

namespace Nom
{
	namespace Runtime
	{
		NomStringClass::NomStringClass() : NomInterface("String_0"), NomClassInternal(new NomString("String_0"))
			//NomClass(NomConstants::AddString(NomString("String")), 0, NomConstants::AddSuperClass(NomConstants::AddClass(NomConstants::AddString(NomString("std")), NomConstants::AddString(NomString("Object"))), 0), 0, nullptr)
		{
			this->SetDirectTypeParameters();
			this->SetSuperClass();
			this->SetSuperInterfaces();

			NomMethodInternal* print = new NomMethodInternal(this, "Print", "LIB_NOM_String_Print_1", true);
			print->SetDirectTypeParameters();
			print->SetArgumentTypes();
			print->SetReturnType(NomVoidClass::GetInstance()->GetType());

			this->AddMethod(print);

			//this->compiled = true;
			////this->preprocessed = true;
			////this->rtclass = &_RTStringClass;
			////NomMethod * print = new NomMethod("Print", NULLNOMTYPE, TypeList());
			//ConstantID voidStringID = NomConstants::AddString("Void");
			//ConstantID stdStringID = NomConstants::AddString("std");
			//ConstantID voidClassID = NomConstants::AddClass(stdStringID, voidStringID);
			//ConstantID voidClassTypeID = NomConstants::AddClassType(voidClassID, 0);
			//NomMethod* print = AddMethod("Print", "LIB_NOM_String_Print_1", 0, voidClassTypeID, 0, 0, true, true, true);
			////MethodTable = NomObjectClass::GetInstance()->MethodTable;
			////print->SetOffset(-1-MethodTable.size());
			////MethodTable.push_back(new NomMethodTableEntry(print, print->GetLLVMFunctionType(), print->GetOffset()));

			//llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_DEBUG_PRINT", (void*)&LIB_NOM_DEBUG_PRINT);
			//llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_DEBUG_PRINT_HEX", (void*)&LIB_NOM_DEBUG_PRINT_HEX);
			//llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_String_Print_1", (void*)&LIB_NOM_String_Print_1);
		}

		NomStringClass* NomStringClass::GetInstance() {
			static NomStringClass nsc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
				NomIntClass::GetInstance();
				NomVoidClass::GetInstance();
				//nsc.PreprocessInheritance();
			}
			return &nsc;
		}
	}
}

extern "C" DLLEXPORT void* LIB_NOM_String_Print_1(void* str)
{
	if (!isInWarmup())
	{
		std::cout << ((NomStringRef)GetReadFieldFunction()(str, 0))->ToStdString();
		std::cout.flush();
	}
	//std::cout << ((NomStringRef)(ObjectHeader(str).Fields()-sizeof(intptr_t)))->ToStdString();
	return (void*)((intptr_t)(NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ")));
}

llvm::Function* GetDebugPrint(llvm::Module* mod)
{
	Function* ret = mod->getFunction("LIB_NOM_DEBUG_PRINT");
	if (ret == nullptr)
	{
		std::array<Type*, 3> chararrpluslen = { { POINTERTYPE, Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), numtype(decltype(NomDebugPrintLevel))} };
		FunctionType* printFunType = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), chararrpluslen, false);
		ret = Function::Create(printFunType, Function::ExternalLinkage, "LIB_NOM_DEBUG_PRINT", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}
llvm::Function* GetDebugPrintHex(llvm::Module* mod)
{
	Function* ret = mod->getFunction("LIB_NOM_DEBUG_PRINT_HEX");
	if (ret == nullptr)
	{
		std::array<Type*, 3> chararrpluslen = { { POINTERTYPE, Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), numtype(decltype(NomDebugPrintLevel))} };
		FunctionType* printFunType = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), chararrpluslen, false);
		ret = Function::Create(printFunType, Function::ExternalLinkage, "LIB_NOM_DEBUG_PRINT_HEX", mod);
		//NomJIT::Instance().RegisterGlobalMapping(ret,(void*) &CPP_NOM_LoadStringConstant);
	}
	return ret;
}