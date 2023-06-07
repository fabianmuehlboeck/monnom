#pragma once
#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalValue.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T>
		class AvailableExternally
		{
		private:
			mutable llvm::Module *mainModule = nullptr;
		public:
			AvailableExternally() {}
			~AvailableExternally() {}
			virtual llvm::Type* GetLLVMElementType(llvm::Module& mod) const;
			T *GetLLVMElement(llvm::Module &mod) const
			{
				T *ret = this->findLLVMElement(mod);
				if (ret == nullptr)
				{
					if (mainModule == nullptr)
					{
						mainModule = &mod;
						ret = this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::ExternalLinkage);
					}
					else
					{
						ret = this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage);
					}
				}
				return ret;
			}
		protected:
			virtual T *createLLVMElement(llvm::Module &mod, llvm::GlobalValue::LinkageTypes linkage) const = 0;
			virtual T *findLLVMElement(llvm::Module &mod) const = 0;
			T *findMainLLVMElement() const {
				if (mainModule != nullptr)
				{
					return findLLVMElement(mainModule);
				}
				return nullptr;
			}
			llvm::Module *GetMainModule() const {
				return mainModule;
			}
		};

	}
}
