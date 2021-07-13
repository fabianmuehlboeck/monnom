#pragma once
#include "RTDictionary.h"
#include "AvailableExternally.h"
#include "llvm/ADT/SmallVector.h"
#include "NomType.h"
namespace Nom
{
	namespace Runtime
	{
		class NomMethod;
		class NomMethodKey : public AvailableExternally<llvm::Function>
		{
		private:
			std::string key;
			const NomMethod* method;
			DICTKEYTYPE name;
			llvm::SmallVector<TypeReferenceType, 8> argTRTs;
			NomMethodKey(std::string&& key, const NomMethod* method, DICTKEYTYPE&& name, llvm::SmallVector<TypeReferenceType, 8>&& argTRTs);
		public:
			NomMethodKey(NomMethodKey& other) = delete;
			NomMethodKey(NomMethodKey&& other) = delete;
			static NomMethodKey* GetMethodKey(const NomMethod* method);
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			const NomMethod* GetMethod() const
			{
				return method;
			}
		};
	}
}