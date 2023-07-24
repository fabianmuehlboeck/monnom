#pragma once
#include "AvailableExternally.h"
PUSHDIAGSUPPRESSION
#include "llvm/IR/Constant.h"
#include "llvm/IR/Module.h"
POPDIAGSUPPRESSION
#include <string>

namespace Nom
{
	namespace Runtime
	{
		class NomRecordCallTag : public AvailableExternally<llvm::Constant>
		{
		private:
			std::string name;
			size_t typeargcount;
			size_t argcount;
			NomRecordCallTag(const std::string &name, size_t typeargcount, size_t argcount);
		public:
			static const NomRecordCallTag* GetCallTag(const std::string &name, size_t typeargcount, size_t argcount);
			virtual ~NomRecordCallTag() override;
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
