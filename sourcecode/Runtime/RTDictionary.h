#pragma once
#include "AvailableExternally.h"
#include "Singleton.h"
#include <unordered_map>
#include "BoehmAllocator.h"
#include "DLLExport.h"

#define DICTKEYTYPE size_t

namespace Nom
{
	namespace Runtime
	{
		using Dicttype = std::unordered_map < DICTKEYTYPE, void*, std::hash<DICTKEYTYPE>, std::equal_to<DICTKEYTYPE>, BoehmAllocator<std::pair<const DICTKEYTYPE, void*>>>;
		size_t GetConcurrentDictionarySize();
		class RTDictionaryCreate : public AvailableExternally<llvm::Function>, public Singleton< RTDictionaryCreate>
		{
			friend class Singleton<RTDictionaryCreate>;
		private:
			RTDictionaryCreate();
		public:
			virtual ~RTDictionaryCreate() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTDictionaryLookup : public AvailableExternally<llvm::Function>, public Singleton< RTDictionaryLookup>
		{
			friend class Singleton<RTDictionaryLookup>;
		private:
			RTDictionaryLookup();
		public:
			virtual ~RTDictionaryLookup() override {}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTDictionarySet : public AvailableExternally<llvm::Function>, public Singleton< RTDictionarySet>
		{
			friend class Singleton<RTDictionarySet>;
		private:
			RTDictionarySet();
		public:
			virtual ~RTDictionarySet() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryCreate : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryCreate>
		{
			friend class Singleton<RTConcurrentDictionaryCreate>;
		private:
			RTConcurrentDictionaryCreate();
		public:
			virtual ~RTConcurrentDictionaryCreate() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryEmplace : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryEmplace>
		{
			friend class Singleton<RTConcurrentDictionaryEmplace>;
		private:
			RTConcurrentDictionaryEmplace();
		public:
			virtual ~RTConcurrentDictionaryEmplace() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryLookup : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryLookup>
		{
			friend class Singleton<RTConcurrentDictionaryLookup>;
		private:
			RTConcurrentDictionaryLookup();
		public:
			virtual ~RTConcurrentDictionaryLookup() override {}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryLookupFreeze : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryLookupFreeze>
		{
			friend class Singleton<RTConcurrentDictionaryLookupFreeze>;
		private:
			RTConcurrentDictionaryLookupFreeze();
		public:
			virtual ~RTConcurrentDictionaryLookupFreeze() override {}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionarySet : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionarySet>
		{
			friend class Singleton<RTConcurrentDictionarySet>;
		private:
			RTConcurrentDictionarySet();
		public:
			virtual ~RTConcurrentDictionarySet() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryGetCastTypeCount : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryGetCastTypeCount>
		{
			friend class Singleton<RTConcurrentDictionaryGetCastTypeCount>;
		private:
			RTConcurrentDictionaryGetCastTypeCount();
		public:
			virtual ~RTConcurrentDictionaryGetCastTypeCount() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryGetCastType : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryGetCastType>
		{
			friend class Singleton<RTConcurrentDictionaryGetCastType>;
		private:
			RTConcurrentDictionaryGetCastType();
		public:
			virtual ~RTConcurrentDictionaryGetCastType() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		class RTConcurrentDictionaryAddCastType : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryAddCastType>
		{
			friend class Singleton<RTConcurrentDictionaryAddCastType>;
		private:
			RTConcurrentDictionaryAddCastType();
		public:
			virtual ~RTConcurrentDictionaryAddCastType() override
			{}

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}

extern "C" DLLEXPORT void* RT_NOM_DictionaryCreate();
extern "C" DLLEXPORT void* RT_NOM_DictionaryLookup(void* dictref, DICTKEYTYPE key);
extern "C" DLLEXPORT void RT_NOM_DictionarySet(void* dictref, DICTKEYTYPE key, void* value);
extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryCreate();
extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryEmplace(void* addr);
extern "C" DLLEXPORT size_t RT_NOM_ConcurrentDictionaryGetCastTypeCount(void* dictref);
extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryGetCastType(void* dictref, size_t index);
extern "C" DLLEXPORT size_t RT_NOM_ConcurrentDictionaryAddCastType(void* dictref, size_t basecount, void* typeref);
extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryLookupFreeze(void* dictref, DICTKEYTYPE key);
extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryLookup(void* dictref, DICTKEYTYPE key);
extern "C" DLLEXPORT int RT_NOM_ConcurrentDictionarySet(void* dictref, DICTKEYTYPE key, void* value);
