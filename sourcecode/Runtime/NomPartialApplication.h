#pragma once
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/ADT/ArrayRef.h"
POPDIAGSUPPRESSION
#include "NomMethod.h"
#include <vector>

namespace Nom
{
	namespace Runtime
	{
		class NomMemberContext;
		class NomPartialApplication : public AvailableExternally<llvm::Constant>
		{
		private:
			std::vector<const NomCallable*> methods;
			const NomMemberContext* context;
			[[maybe_unused]] NomTypeRef thisType;
		public:
			const std::string SymbolName;
			NomPartialApplication(const std::string symbolName, llvm::ArrayRef<const NomCallable*> methods, const NomMemberContext* context, NomTypeRef thisType);
			static llvm::FunctionType* GetDynamicDispatcherType();
			static llvm::Function* GetDispatcherEntry(llvm::Module& mod, NomTypeRef thisType, llvm::GlobalValue::LinkageTypes linkage, llvm::ArrayRef<const NomCallable*> overloadings, const NomMemberContext* context);
			virtual ~NomPartialApplication() override {}
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
