#pragma once
#include "NomClass.h"
#include "NomVMInterface.h"

namespace Nom
{
	namespace Runtime
	{
		class NomIntClass : public NomClassInternal
		{
		private:
			NomIntClass();
		public:
			static NomIntClass *GetInstance();
			virtual ~NomIntClass() override;
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
		};
	}
}

extern "C" DLLEXPORT const void * LIB_NOM_Int_ToString_1(int64_t value);
extern "C" DLLEXPORT int64_t LIB_NOM_Int_Compare_1(const int64_t value, const int64_t other);
