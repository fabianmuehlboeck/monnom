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


		//class RTIntClass : public RTClass
		//{
		//public:
		//	RTIntClass();
		//	~RTIntClass() {}
		//};
	}
}

//extern const Nom::Runtime::RTIntClass _RTIntClass;
//extern const Nom::Runtime::NomIntClass _NomIntClass;
//extern const Nom::Runtime::NomIntClass * const _NomIntClassRef;
//extern const Nom::Runtime::RTIntClass * const _RTIntClassRef;
//extern const Nom::Runtime::NomClass * const _NomIntClassNC;
//extern const Nom::Runtime::RTClass * const _RTIntClassRTC;

extern "C" DLLEXPORT const void * LIB_NOM_Int_ToString_1(int64_t value);