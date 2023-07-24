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
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include "IntClass.h"
#include "CastStats.h"
#include "NomClassType.h"
#include "Runtime.h"

using namespace Nom::Runtime;

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
		debugPrint_lastStrs[level][debugPrint_rbufpos] = static_cast<std::string*>(str);
		debugPrint_rbufpos = (debugPrint_rbufpos + 1) % 100;
	}
	if (NomDebugPrintLevel >= level)
	{
		std::cout << static_cast<std::string*>(str);
		std::cout << ":";
		std::cout << std::dec << index;
		std::cout << "\n";
		std::cout.flush();
	}
	return;
}
extern "C" DLLEXPORT void LIB_NOM_DEBUG_PRINT_HEX(void* str, int64_t index, decltype(NomDebugPrintLevel) level)
{
	if (level <= 3)
	{
		debugPrintX_lastLevel = level;
		debugPrintX_lastIndices[level][debugPrintX_rbufpos] = index;
		debugPrintX_lastStrs[level][debugPrintX_rbufpos] = static_cast<std::string*>(str);
		debugPrintX_rbufpos = (debugPrintX_rbufpos + 1) % 100;
	}
	if (NomDebugPrintLevel >= level)
	{
		std::cout << static_cast<std::string*>(str);
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
		NomStringClass::NomStringClass() : NomInterface(), NomClassInternal(new NomString("String_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperClass();
			this->SetSuperInterfaces();

			NomMethodInternal* print = new NomMethodInternal(this, "Print", "LIB_NOM_String_Print_1", true);
			print->SetDirectTypeParameters();
			print->SetArgumentTypes();
			print->SetReturnType(NomVoidClass::GetInstance()->GetType());

			this->AddMethod(print);
		}

		NomStringClass* NomStringClass::GetInstance() {
			[[clang::no_destroy]] static NomStringClass nsc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
				NomIntClass::GetInstance();
				NomVoidClass::GetInstance();
			}
			return &nsc;
		}
	}
}

extern "C" DLLEXPORT void* LIB_NOM_String_Print_1(void* str)
{
	if (!isInWarmup())
	{
		std::cout << (static_cast<NomStringRef>(GetReadFieldFunction()(str, 0)))->ToStdString();
		std::cout.flush();
	}
	return reinterpret_cast<void*>(NomJIT::Instance().getSymbolAddress("RT_NOM_VOIDOBJ"));
}

llvm::Function* GetDebugPrint(llvm::Module* mod)
{
	Function* ret = mod->getFunction("LIB_NOM_DEBUG_PRINT");
	if (ret == nullptr)
	{
		std::array<Type*, 3> chararrpluslen = { { POINTERTYPE, Type::getIntNTy(LLVMCONTEXT, bitsin(uint64_t)), numtype(decltype(NomDebugPrintLevel))} };
		FunctionType* printFunType = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), chararrpluslen, false);
		ret = Function::Create(printFunType, Function::ExternalLinkage, "LIB_NOM_DEBUG_PRINT", mod);
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
	}
	return ret;
}
