#pragma once
#include "NomClass.h"

namespace Nom
{
	namespace Runtime
	{
		class NomFloatClass : public NomClassInternal
		{
		private:
			NomFloatClass();
		public:
			static NomFloatClass *GetInstance();
			virtual ~NomFloatClass() override;
			virtual void GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const override;
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;
		};

		class NomFloatObjects : public AvailableExternally < llvm::Constant >
		{
		private:
			NomFloatObjects();
		public:
			static NomFloatObjects* GetInstance() { static NomFloatObjects obj; return &obj; }
			~NomFloatObjects() {}

			static llvm::Constant* GetPosZero(llvm::Module& mod);
			static llvm::Constant* GetNegZero(llvm::Module& mod);

			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}
