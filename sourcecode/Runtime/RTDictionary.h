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
			virtual ~RTDictionaryCreate()
			{};

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
			virtual ~RTDictionaryLookup() {};

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
			virtual ~RTDictionarySet()
			{};

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
			virtual ~RTConcurrentDictionaryCreate()
			{};

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
			virtual ~RTConcurrentDictionaryEmplace()
			{};

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
			virtual ~RTConcurrentDictionaryLookup() {};

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
			virtual ~RTConcurrentDictionaryLookupFreeze() {};

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
			virtual ~RTConcurrentDictionarySet()
			{};

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
			virtual ~RTConcurrentDictionaryGetCastTypeCount()
			{};

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
			virtual ~RTConcurrentDictionaryGetCastType()
			{};

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};

		//class RTConcurrentDictionaryEnterCast : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryEnterCast>
		//{
		//	friend class Singleton<RTConcurrentDictionaryEnterCast>;
		//private:
		//	RTConcurrentDictionaryEnterCast();
		//public:
		//	virtual ~RTConcurrentDictionaryEnterCast()
		//	{};

		//	static llvm::FunctionType* GetFunctionType();
		//	// Inherited via AvailableExternally
		//	virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		//	virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		//};

		class RTConcurrentDictionaryAddCastType : public AvailableExternally<llvm::Function>, public Singleton< RTConcurrentDictionaryAddCastType>
		{
			friend class Singleton<RTConcurrentDictionaryAddCastType>;
		private:
			RTConcurrentDictionaryAddCastType();
		public:
			virtual ~RTConcurrentDictionaryAddCastType()
			{};

			static llvm::FunctionType* GetFunctionType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}

extern "C" DLLEXPORT void* RT_NOM_ConcurrentDictionaryEmplace(void* addr);
