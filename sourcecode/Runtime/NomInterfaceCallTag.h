#pragma once
#include "RTDictionary.h"
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/ADT/SmallVector.h"
POPDIAGSUPPRESSION
#include "NomType.h"
namespace Nom
{
	namespace Runtime
	{
		class NomMethod;
		class NomInterfaceCallTag : public AvailableExternally<llvm::Function>
		{
		private:
			std::string key;
			const NomMethod* method;
			DICTKEYTYPE name;
			llvm::SmallVector<TypeReferenceType, 8> argTRTs;
			NomInterfaceCallTag(std::string&& key, const NomMethod* method, DICTKEYTYPE&& name, llvm::SmallVector<TypeReferenceType, 8>&& argTRTs);
		public:
			NomInterfaceCallTag(NomInterfaceCallTag& other) = delete;
			NomInterfaceCallTag(NomInterfaceCallTag&& other) = delete;
			static NomInterfaceCallTag* GetMethodKey(const NomMethod* method);
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
