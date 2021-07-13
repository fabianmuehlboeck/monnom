#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "RTTypes.h"
#include "llvm/IR/Constant.h"
#include "AvailableExternally.h"

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
			//static llvm::GlobalVariable * VoidObjectVar(llvm::Module &mod);
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
		//class RTVoidClass : public RTClass
		//{
		//public:
		//	RTVoidClass() : RTClass(0, 0)
		//	{
		//	}
		//};
	}
}

//extern const Nom::Runtime::RTVoidClass _RTVoidClass;

//extern "C" const Nom::Runtime::ObjectHeader * const VOIDOBJ;
//extern "C" const Nom::Runtime::RTClassType VOIDCLASSTYPE;
//extern "C" const Nom::Runtime::RTTypeHead VOIDTYPE;
extern "C" const Nom::Runtime::NomType * const VOIDNOMTYPE;

//extern const Nom::Runtime::NomVoidClass _NomVoidClass;