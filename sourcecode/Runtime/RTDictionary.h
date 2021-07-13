#pragma once
#include "AvailableExternally.h"
#include "Singleton.h"
#include <unordered_map>
#include "BoehmAllocator.h"

#define DICTKEYTYPE size_t

namespace Nom
{
	namespace Runtime
	{
		using Dicttype = std::unordered_map < DICTKEYTYPE, void*, std::hash<DICTKEYTYPE>, std::equal_to<DICTKEYTYPE>, BoehmAllocator<std::pair<const DICTKEYTYPE, void*>>>;
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
	}
}