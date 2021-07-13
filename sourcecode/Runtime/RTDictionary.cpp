#include "RTDictionary.h"
//#include "llvm/ADT/DenseMap.h"
#include <unordered_map>
#include "Defs.h"
#include "NomVMInterface.h"
#include "NomAlloc.h"
#include "BoehmAllocator.h"
#include "NomNameRepository.h"
#include <iostream>
#include "PreparedDictionary.h"
#include "tbb/concurrent_unordered_map.h"
#include "llvm/Support/DynamicLibrary.h"
#include <functional>

using namespace std;
using namespace llvm;
using namespace Nom::Runtime;

namespace Nom
{
	namespace Runtime
	{
		using ConcurrentDictType = tbb::concurrent_unordered_map< DICTKEYTYPE, void*, std::hash<DICTKEYTYPE>, std::equal_to<DICTKEYTYPE>, BoehmAllocator<std::pair<const DICTKEYTYPE, void*>>>;

	}
}


extern "C" DLLEXPORT void* RT_NOM_DictionaryCreate()
{
	Dicttype* dict = new(gcalloc(sizeof(Dicttype))) Dicttype();
	return dict;
}

extern "C" DLLEXPORT void* RT_NOM_DictionaryLookup(void* dictref, DICTKEYTYPE key)
{
	Dicttype* dict = (Dicttype*)dictref;
	auto result = dict->find(key);
	if (result == dict->end())
	{
		std::cout << "Dictionary entry not found: " << *(NomNameRepository::Instance().GetNameFromID(key));
		return nullptr;
	}
	else
	{
		return result->second;
	}
}

extern "C" DLLEXPORT void RT_NOM_DictionarySet(void* dictref, DICTKEYTYPE key, void* value)
{
	Dicttype* dict = (Dicttype*)dictref;
	(*dict)[key] = value;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryCreate()
{
	ConcurrentDictType* dict = new(gcalloc(sizeof(ConcurrentDictType))) ConcurrentDictType();
	return dict;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryLookup(void* dictref, DICTKEYTYPE key)
{
	ConcurrentDictType* dict = (ConcurrentDictType*)dictref;
	auto result = dict->find(key);
	if (result == dict->end())
	{
		std::cout << "Dictionary entry not found: " << *(NomNameRepository::Instance().GetNameFromID(key));
		return nullptr;
	}
	else
	{
		return result->second;
	}
}

extern "C" DLLEXPORT void RT_NOM_ConcurrentDictionarySet(void* dictref, DICTKEYTYPE key, void* value)
{
	ConcurrentDictType* dict = (ConcurrentDictType*)dictref;
	(*dict)[key] = value;
}
namespace Nom
{
	namespace Runtime
	{
		RTDictionaryCreate::RTDictionaryCreate()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionaryCreate", (void*)& RT_NOM_DictionaryCreate);
		}
		llvm::FunctionType* RTDictionaryCreate::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, {}, false);
			return ft;
		}
		llvm::Function* RTDictionaryCreate::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DictionaryCreate", &mod);
			return fun;
		}
		llvm::Function* RTDictionaryCreate::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_DictionaryCreate");
		}

		RTDictionaryLookup::RTDictionaryLookup()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionaryLookup", (void*)& RT_NOM_DictionaryLookup);
		}

		llvm::FunctionType* RTDictionaryLookup::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, { POINTERTYPE, numtype(DICTKEYTYPE) }, false);
			return ft;
		}

		llvm::Function* RTDictionaryLookup::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DictionaryLookup", &mod);
			return fun;
		}
		llvm::Function* RTDictionaryLookup::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_DictionaryLookup");
		}

		RTDictionarySet::RTDictionarySet()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionarySet", (void*)& RT_NOM_DictionarySet);
		}

		llvm::FunctionType* RTDictionarySet::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { POINTERTYPE, numtype(DICTKEYTYPE), POINTERTYPE }, false);
			return ft;
		}

		llvm::Function* RTDictionarySet::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_DictionarySet", &mod);
			return fun;
		}
		llvm::Function* RTDictionarySet::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_DictionarySet");
		}


		RTConcurrentDictionaryCreate::RTConcurrentDictionaryCreate()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionaryCreate", (void*)& RT_NOM_ConcurrentDictionaryCreate);
		}

		llvm::FunctionType* RTConcurrentDictionaryCreate::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, {}, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryCreate::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryCreate", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryCreate::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryCreate");
		}

		RTConcurrentDictionaryLookup::RTConcurrentDictionaryLookup()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionaryLookup", (void*)& RT_NOM_ConcurrentDictionaryLookup);
		}

		llvm::FunctionType* RTConcurrentDictionaryLookup::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, { POINTERTYPE, numtype(DICTKEYTYPE) }, false);
			return ft;
		}

		llvm::Function* RTConcurrentDictionaryLookup::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryLookup", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryLookup::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryLookup");
		}

		RTConcurrentDictionarySet::RTConcurrentDictionarySet()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionarySet", (void*)& RT_NOM_ConcurrentDictionarySet);
		}

		llvm::FunctionType* RTConcurrentDictionarySet::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { POINTERTYPE, numtype(DICTKEYTYPE), POINTERTYPE }, false);
			return ft;
		}

		llvm::Function* RTConcurrentDictionarySet::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionarySet", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionarySet::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionarySet");
		}
	}
}
