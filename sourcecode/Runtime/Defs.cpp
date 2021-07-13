#include "Defs.h"
#include "ObjectHeader.h"
#include "StringClass.h"
#include "NomAlloc.h"
#include "NomJIT.h"
#include <iostream>
#include "llvm/Support/raw_os_ostream.h"
#include "GlobalNameAddressLookupList.h"
#include "BoolClass.h"
#include "instructions/CastInstruction.h"
#include "NomClassType.h"
#include "RefValueHeader.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::PointerType * BytecodeTypes::GetRefType() {
			//static llvm::Type * refType = (llvm::StructType::create(Nom::Runtime::TheContext)->getPointerTo());
			static llvm::PointerType *refType = RefValueHeader::GetLLVMType()->getPointerTo();
			return refType;
		}

		llvm::PointerType * BytecodeTypes::GetTypeType()
		{
			static llvm::PointerType* typeType = RTTypeHead::GetLLVMType()->getPointerTo();
			return typeType;
		}


		std::vector<std::string> globalsForAddressLookup;
		void RegisterGlobalForAddressLookup(std::string name)
		{
			globalsForAddressLookup.push_back(name);
		}
		std::vector<std::string> &GetGlobalsForAddressLookup()
		{
			return globalsForAddressLookup;
		}
	}
}

extern "C" DLLEXPORT void RT_NOM_PRINT_STORE(void* val, void* addr)
{
	std::cout << "(";
	std::cout << std::hex << (intptr_t)val;
	std::cout << "->";
	std::cout << std::hex << (intptr_t)addr;
	std::cout << ")";
	std::cout.flush();
}

extern "C" DLLEXPORT void RT_NOM_PRINT_LOAD(void* val, void* addr)
{
	std::cout << "(";
	std::cout << std::hex << (intptr_t)val;
	std::cout << "<-";
	std::cout << std::hex << (intptr_t)addr;
	std::cout << ")";
	std::cout.flush();
}