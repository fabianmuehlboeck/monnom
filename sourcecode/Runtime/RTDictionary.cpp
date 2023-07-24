#include "RTDictionary.h"
#include <unordered_map>
#include "Defs.h"
#include "NomVMInterface.h"
#include "NomAlloc.h"
#include "BoehmAllocator.h"
#include "NomNameRepository.h"
#include <iostream>
#include <functional>
#include <vector>
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
#include "tbb/concurrent_hash_map.h"
POPDIAGSUPPRESSION

#ifdef __clang__
#pragma clang diagnostic ignored "-Wexceptions"
#elif defined(__GNU__)
#pragma GCC diagnostic ignored "-Wexceptions"
#elif defined(_MSC_VER)

#endif

using namespace std;
using namespace llvm;
using namespace Nom::Runtime;

namespace Nom
{
	namespace Runtime
	{
		template <typename DKT>
		struct DKT_hashCompare
		{
		public:
			DKT_hashCompare() {}
			DKT_hashCompare(DKT_hashCompare& dkthc) = default;
			DKT_hashCompare(const DKT_hashCompare & dkthc) = default;
			~DKT_hashCompare() = default;
			bool equal(const DKT& j, const DKT& k) const
			{
				return std::equal_to<DKT>()(j, k);
			}
			size_t hash(const DKT& k) const
			{
				return std::hash<DKT>()(k);
			}
		};
		using ConcurrentDictType = tbb::concurrent_hash_map< DICTKEYTYPE, void*, DKT_hashCompare<DICTKEYTYPE>, BoehmAllocator<std::pair<const DICTKEYTYPE, void*>>>;
		size_t GetConcurrentDictionarySize()
		{
			return sizeof(ConcurrentDictType);
		}
		static DICTKEYTYPE GetTypeEntryKey(size_t index)
		{
			[[clang::no_destroy]] static std::vector<size_t> typeEntryKeys;
			while (typeEntryKeys.size() <= index)
			{
				typeEntryKeys.push_back(NomNameRepository::Instance().GetNameID("%CASTTYPE" + std::to_string(typeEntryKeys.size())));
			}
			return typeEntryKeys[index];
		}
	}
}


extern "C" DLLEXPORT void* RT_NOM_DictionaryCreate()
{
	Dicttype* dict = new(gcalloc(sizeof(Dicttype))) Dicttype();
	return dict;
}

extern "C" DLLEXPORT void* RT_NOM_DictionaryLookup(void* dictref, DICTKEYTYPE key)
{
	Dicttype* dict = static_cast<Dicttype*>(dictref);
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
	Dicttype* dict = static_cast<Dicttype*>(dictref);
	(*dict)[key] = value;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryCreate()
{
	ConcurrentDictType* dict = new(gcalloc(sizeof(ConcurrentDictType))) ConcurrentDictType();
	return dict;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryEmplace(void* addr)
{
	ConcurrentDictType* dict = new(addr) ConcurrentDictType();
	return dict;
}

extern "C" DLLEXPORT size_t RT_NOM_ConcurrentDictionaryGetCastTypeCount(void* dictref)
{
	static DICTKEYTYPE key = NomNameRepository::Instance().GetNameID("%CASTTYPECOUNT");
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::const_accessor acc;
	if (dict->find(acc, key))
	{
		return reinterpret_cast<size_t>(acc->second);
	}
	return 0;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryGetCastType(void* dictref, size_t index)
{
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::const_accessor acc;
	if (dict->find(acc, GetTypeEntryKey(index)))
	{
		return acc->second;
	}
	return nullptr;
}

extern "C" DLLEXPORT size_t RT_NOM_ConcurrentDictionaryAddCastType(void* dictref, size_t basecount, void* typeref)
{
	static DICTKEYTYPE key = NomNameRepository::Instance().GetNameID("%CASTTYPECOUNT");
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::accessor acc;
	ConcurrentDictType::accessor eAcc;
	if (dict->insert(acc, key))
	{
		acc->second = reinterpret_cast<void*>(0);
	}
	if ((reinterpret_cast<size_t>(acc->second)) != basecount)
	{
		return (reinterpret_cast<size_t>(acc->second));
	}
	if (!dict->insert(eAcc, GetTypeEntryKey(reinterpret_cast<size_t>(acc->second))))
	{
		throw new std::exception();
	}
	eAcc->second = typeref;
	acc->second = reinterpret_cast<void*> ((reinterpret_cast<size_t>(acc->second))+1);
	return basecount;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryLookupFreeze(void* dictref, DICTKEYTYPE key)
{
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::accessor acc;
	void* ret = nullptr;
	if (dict->find(acc, key))
	{
		ret = acc->second;
		acc->second = reinterpret_cast<void*>((reinterpret_cast<intptr_t>(acc->second)) | 4);
	}
	return ret;
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryLookup(void* dictref, DICTKEYTYPE key)
{
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::const_accessor acc;
	if (dict->find(acc, key))
	{
		return acc->second;
	}
#ifdef _DEBUG
	std::cout << "Dictionary entry not found: " << *(NomNameRepository::Instance().GetNameFromID(key));
#endif
	return nullptr;
}

extern "C" DLLEXPORT int RT_NOM_ConcurrentDictionarySet(void* dictref, DICTKEYTYPE key, void* value)
{
	ConcurrentDictType* dict = static_cast<ConcurrentDictType*>(dictref);
	ConcurrentDictType::accessor acc;
	if (!dict->insert(acc, key))
	{
		if (((reinterpret_cast<intptr_t>(acc->second)) & (static_cast<intptr_t>(7))) == static_cast<intptr_t>(4))
		{
			std::cout << "Tried to set frozen dictionary value!\n";
			return false;
		}
	}
	acc->second = value;
	return true;
}
namespace Nom
{
	namespace Runtime
	{
		RTDictionaryCreate::RTDictionaryCreate()
		{
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionaryCreate", reinterpret_cast<void*>(& RT_NOM_DictionaryCreate));
		}
		llvm::FunctionType* RTDictionaryCreate::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, {}, false);
			return ft;
		}
		llvm::Function* RTDictionaryCreate::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
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
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionaryLookup", reinterpret_cast<void*>(& RT_NOM_DictionaryLookup));
		}

		llvm::FunctionType* RTDictionaryLookup::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, { POINTERTYPE, numtype(DICTKEYTYPE) }, false);
			return ft;
		}

		llvm::Function* RTDictionaryLookup::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
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
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_DictionarySet", reinterpret_cast<void*>(& RT_NOM_DictionarySet));
		}

		llvm::FunctionType* RTDictionarySet::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(Type::getVoidTy(LLVMCONTEXT), { POINTERTYPE, numtype(DICTKEYTYPE), POINTERTYPE }, false);
			return ft;
		}

		llvm::Function* RTDictionarySet::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
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
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionaryCreate", reinterpret_cast<void*>(& RT_NOM_ConcurrentDictionaryCreate));
		}

		llvm::FunctionType* RTConcurrentDictionaryCreate::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, {}, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryCreate::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
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
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionaryLookup", reinterpret_cast<void*>(& RT_NOM_ConcurrentDictionaryLookup));
		}

		llvm::FunctionType* RTConcurrentDictionaryLookup::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, { POINTERTYPE, numtype(DICTKEYTYPE) }, false);
			return ft;
		}

		llvm::Function* RTConcurrentDictionaryLookup::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
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
			llvm::sys::DynamicLibrary::AddSymbol("RT_NOM_ConcurrentDictionarySet", reinterpret_cast<void*>(& RT_NOM_ConcurrentDictionarySet));
		}

		llvm::FunctionType* RTConcurrentDictionarySet::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(numtype(int), { POINTERTYPE, numtype(DICTKEYTYPE), POINTERTYPE }, false);
			return ft;
		}

		llvm::Function* RTConcurrentDictionarySet::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionarySet", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionarySet::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionarySet");
		}


		RTConcurrentDictionaryEmplace::RTConcurrentDictionaryEmplace()
		{
		}

		llvm::FunctionType* RTConcurrentDictionaryEmplace::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, {POINTERTYPE}, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryEmplace::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryEmplace", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryEmplace::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryEmplace");
		}
		RTConcurrentDictionaryLookupFreeze::RTConcurrentDictionaryLookupFreeze()
		{
		}
		llvm::FunctionType* RTConcurrentDictionaryLookupFreeze::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(POINTERTYPE, { POINTERTYPE, numtype(DICTKEYTYPE) }, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryLookupFreeze::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryLookupFreeze", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryLookupFreeze::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryLookupFreeze");
		}


		RTConcurrentDictionaryGetCastTypeCount::RTConcurrentDictionaryGetCastTypeCount()
		{
		}

		llvm::FunctionType* RTConcurrentDictionaryGetCastTypeCount::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(numtype(size_t), { POINTERTYPE }, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryGetCastTypeCount::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryGetCastTypeCount", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryGetCastTypeCount::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryGetCastTypeCount");
		}

		RTConcurrentDictionaryGetCastType::RTConcurrentDictionaryGetCastType()
		{
		}

		llvm::FunctionType* RTConcurrentDictionaryGetCastType::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(TYPETYPE, { POINTERTYPE, numtype(size_t) }, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryGetCastType::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryGetCastType", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryGetCastType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryGetCastType");
		}

		RTConcurrentDictionaryAddCastType::RTConcurrentDictionaryAddCastType()
		{
		}

		llvm::FunctionType* RTConcurrentDictionaryAddCastType::GetFunctionType()
		{
			static FunctionType* ft = FunctionType::get(numtype(size_t), { POINTERTYPE, INTTYPE, TYPETYPE }, false);
			return ft;
		}
		llvm::Function* RTConcurrentDictionaryAddCastType::createLLVMElement(llvm::Module& mod, [[maybe_unused]] llvm::GlobalValue::LinkageTypes linkage) const
		{
			Function* fun = Function::Create(GetFunctionType(), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "RT_NOM_ConcurrentDictionaryAddCastType", &mod);
			return fun;
		}
		llvm::Function* RTConcurrentDictionaryAddCastType::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getFunction("RT_NOM_ConcurrentDictionaryAddCastType");
		}
	}
}
