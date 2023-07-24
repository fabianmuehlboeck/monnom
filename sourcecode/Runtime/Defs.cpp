PUSHDIAGSUPPRESSION
#include "llvm/Support/raw_os_ostream.h"
POPDIAGSUPPRESSION
#include "Defs.h"
#include "ObjectHeader.h"
#include "StringClass.h"
#include "NomAlloc.h"
#include "NomJIT.h"
#include <iostream>
#include "GlobalNameAddressLookupList.h"
#include "BoolClass.h"
#include "instructions/CastInstruction.h"
#include "NomClassType.h"
#include "RefValueHeader.h"

namespace Nom
{
	namespace Runtime
	{
		NLLVMPointer &BytecodeTypes::GetRefType() {
			//static llvm::Type * refType = (llvm::StructType::create(Nom::Runtime::TheContext)->getPointerTo());
			static NLLVMPointer refType = NLLVMPointer(RefValueHeader::GetLLVMType());
			return refType;
		}

		NLLVMPointer &BytecodeTypes::GetTypeType()
		{
			static NLLVMPointer typeType = NLLVMPointer(RTTypeHead::GetLLVMType());
			return typeType;
		}


		[[clang::no_destroy]] static std::vector<std::string> globalsForAddressLookup;
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
	std::cout << std::hex << reinterpret_cast<intptr_t>(val);
	std::cout << "->";
	std::cout << std::hex << reinterpret_cast<intptr_t>(addr);
	std::cout << ")";
	std::cout.flush();
}

extern "C" DLLEXPORT void RT_NOM_PRINT_LOAD(void* val, void* addr)
{
	std::cout << "(";
	std::cout << std::hex << reinterpret_cast<intptr_t>(val);
	std::cout << "<-";
	std::cout << std::hex << reinterpret_cast<intptr_t>(addr);
	std::cout << ")";
	std::cout.flush();
}
