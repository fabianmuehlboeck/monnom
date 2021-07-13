#pragma once
#include "ARTRep.h"
#include "RTVTable.h"
#include "AvailableExternally.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTMulticastedInterfaceFields :unsigned char { RTInterface = 0, OrigInterface = 1, TypeArgs = 2 };
		class RTMulticastedInterface : public ARTRep<RTMulticastedInterface, RTMulticastedInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage);
		};

		class RTMulticastIMT : public AvailableExternally<llvm::Function>
		{
		public:
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
			static RTMulticastIMT& Instance();
		};

		class RTMulticast : public AvailableExternally<llvm::Function>
		{

		public:
			static llvm::FunctionType* GetLLVMType();
			// Inherited via AvailableExternally
			virtual llvm::Function* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Function* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}