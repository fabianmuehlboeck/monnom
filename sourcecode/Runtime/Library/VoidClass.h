#pragma once
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomClass.h"
#include "llvm/IR/Constant.h"
#include "../Intermediate_Representation/AvailableExternally.h"

namespace Nom
{
	namespace Runtime
	{

		class NomVoidClass : public NomClassInternal
		{
		private:
			NomVoidClass();
		public:
			virtual ~NomVoidClass() override;
			static NomVoidClass *GetInstance();
			static void * VoidObject();
		};

		class NomVoidObject : public AvailableExternally<llvm::Constant>
		{
		private:
			NomVoidObject() {}
		public:
			static NomVoidObject* GetInstance();
			~NomVoidObject() {}
			// Inherited via AvailableExternally
			virtual llvm::Constant * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant * findLLVMElement(llvm::Module & mod) const override;
		};
	}
}

extern "C" const Nom::Runtime::NomType * const VOIDNOMTYPE;
