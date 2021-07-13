#pragma once
#include "AvailableExternally.h"
#include "llvm/ADT/ArrayRef.h"
#include "NomMethod.h"
#include <vector>
#include "NomDescriptor.h"

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
			NomTypeRef thisType;
		public:
			const std::string SymbolName;
			NomPartialApplication(const std::string symbolName, llvm::ArrayRef<const NomCallable*> methods, const NomMemberContext* context, NomTypeRef thisType);
			static llvm::FunctionType* GetDynamicDispatcherType(uint32_t typeargcount, uint32_t argcount);
			static llvm::Function* GetDispatcherEntry(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, int32_t typeArgCount, int32_t argCount, llvm::ArrayRef<const NomCallable*> overloadings, const NomMemberContext* context, NomTypeRef thisType);
			virtual ~NomPartialApplication() {}
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}