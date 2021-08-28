#pragma once
#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalValue.h"

namespace Nom
{
	namespace Runtime
	{
		template<typename T, typename S>
		class AvailableExternallyVariants
		{
		private:
			mutable llvm::Module* mainModule = nullptr;
		public:
			AvailableExternallyVariants() {}
			~AvailableExternallyVariants() {}

			T* GetLLVMElement(llvm::Module& mod, S &variant) const
			{
				T* ret = this->findLLVMElement(mod, variant);
				if (ret == nullptr)
				{
					if (mainModule == nullptr)
					{
						mainModule = &mod;
						bool found = false;
						for (auto& var : getVariants())
						{
							if (var == variant)
							{
								ret = this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::ExternalLinkage, var);
								found = true;
							}
							else
							{
								this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::ExternalLinkage, var);
							}
						}
						if (!found)
						{
							ret = this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::ExternalLinkage, variant);
						}
					}
					else
					{
						ret = this->createLLVMElement(mod, llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage, variant);
					}
				}
				return ret;
			}
		protected:
			virtual T* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, S& variant) const = 0;
			virtual T* findLLVMElement(llvm::Module& mod, S& variant) const = 0;
			virtual llvm::ArrayRef<S> getVariants() const = 0;
			T* findMainLLVMElement(S& variant) const {
				if (mainModule != nullptr)
				{
					return findLLVMElement(mainModule, variant);
				}
				return nullptr;
			}
			llvm::Module* GetMainModule() const {
				return mainModule;
			}
		};

	}
}
