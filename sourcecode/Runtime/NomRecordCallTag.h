#pragma once
#include "AvailableExternally.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Module.h"
#include <string>

namespace Nom
{
	namespace Runtime
	{
		class NomRecordCallTag : public AvailableExternally<llvm::Constant>
		{
		private:
			std::string name;
			int typeargcount;
			int argcount;
			NomRecordCallTag(const std::string &name, int typeargcount, int argcount);
		public:
			static const NomRecordCallTag* GetCallTag(const std::string &name, int typeargcount, int argcount);
			~NomRecordCallTag();
			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}