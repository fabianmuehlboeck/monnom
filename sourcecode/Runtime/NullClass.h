#pragma once
#include "NomClass.h"
#include "RTClass.h"
#include "RTClassType.h"
#include "NomConstants.h"
#include "RTTypeHead.h"
#include "AvailableExternally.h"
#include "llvm/IR/Constant.h"

namespace Nom
{
	namespace Runtime
	{
		class ObjectHeader;
		class NomType;
		//class RTNullClass : public RTClass
		//{
		//public:
		//	RTNullClass() : RTClass(0, 0)
		//	{
		//	}
		//};
	}
}

//extern const Nom::Runtime::RTNullClass _RTNullClass;

//extern "C" const Nom::Runtime::ObjectHeader const NULLOBJ;
//extern "C" const Nom::Runtime::RTClassType NULLCLASSTYPE;
//extern "C" const Nom::Runtime::RTTypeHead NULLTYPE;
//extern "C" const Nom::Runtime::NomType * const NULLNOMTYPE;

namespace Nom
{
	namespace Runtime
	{
		class NomNullClass : public NomClassInternal//, public AvailableExternally<llvm::GlobalVariable>
		{
		private:
			NomNullClass();
		public:
			//static llvm::GlobalVariable *NullObjectVar(llvm::Module &mod);
			static void * NullObject();
			static NomNullClass *GetInstance();
			virtual ~NomNullClass() override
			{

			}

			// Inherited via AvailableExternally
			//virtual llvm::GlobalVariable * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			//virtual llvm::GlobalVariable * findLLVMElement(llvm::Module & mod) const override;
		};
		class NomNullObject : public AvailableExternally < llvm::Constant >
		{
		private:
			NomNullObject();
		public:
			static NomNullObject *GetInstance() { static NomNullObject obj; return &obj; }
			~NomNullObject() {}

			// Inherited via AvailableExternally
			virtual llvm::Constant * createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant * findLLVMElement(llvm::Module & mod) const override;
		};
	}
}

//extern const Nom::Runtime::NomNullClass _NomNullClass;