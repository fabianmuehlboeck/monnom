#pragma once
#include "NomClass.h"
//#include "RTClass.h"
namespace Nom
{
	namespace Runtime
	{
		class NomBoolClass : public NomClassInternal
		{
		private:
			NomBoolClass();
		public:
			static NomBoolClass* GetInstance();
			virtual ~NomBoolClass() override;
		};

		//class RTBoolClass : public RTClass
		//{
		//public:
		//	RTBoolClass();
		//	~RTBoolClass() {}
		//};
		class NomBoolObjects : public AvailableExternally < llvm::Constant >
		{
		private:
			NomBoolObjects();
		public:
			static NomBoolObjects* GetInstance() { static NomBoolObjects obj; return &obj; }
			~NomBoolObjects() {}

			static llvm::Constant* GetTrue(llvm::Module& mod);
			static llvm::Constant* GetFalse(llvm::Module& mod);

			static llvm::Value* PackBool(NomBuilder& builder, llvm::Value* b);
			static llvm::Value* UnpackBool(NomBuilder& builder, llvm::Value* b);



			// Inherited via AvailableExternally
			virtual llvm::Constant* createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const override;
			virtual llvm::Constant* findLLVMElement(llvm::Module& mod) const override;
		};

	}
}


////extern const Nom::Runtime::RTBoolClass _RTBoolClass;
//extern const Nom::Runtime::NomBoolClass _NomBoolClass;
//extern const Nom::Runtime::NomBoolClass * const _NomBoolClassRef;
////extern const Nom::Runtime::RTBoolClass * const _RTBoolClassRef;
//extern const Nom::Runtime::NomClass * const _NomBoolClassNC;
////extern const Nom::Runtime::RTClass * const _RTBoolClassRTC;