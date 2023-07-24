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
		};

		class NomFloatObjects : public AvailableExternally < llvm::Constant >
		{
		private:
			NomFloatObjects();
		public:
			static NomFloatObjects* GetInstance() { [[clang::no_destroy]] static NomFloatObjects obj; return &obj; }
			~NomFloatObjects() override {}

			static llvm::Constant* GetPosZero(llvm::Module& mod);
			static llvm::Constant* GetNegZero(llvm::Module& mod);

			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};
	}
}

extern "C" DLLEXPORT const void* LIB_NOM_Float_ToString_1(const double value);
extern "C" DLLEXPORT int64_t LIB_NOM_Float_Compare_1(const double value, const double other);
