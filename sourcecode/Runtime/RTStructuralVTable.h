#pragma once
#include "ARTRep.h"
#include "AvailableExternally.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Module.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTStructuralVTableFields :unsigned char { TypeArgs = 0, RTInterface = 1, OrigInterface = 2 };
		class RTStructuralVTable : public AvailableExternally<llvm::Constant>
		{
		private:
			RTStructuralVTable();
		public:
			static RTStructuralVTable& Instance();
			static llvm::StructType* GetLLVMType();

			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}