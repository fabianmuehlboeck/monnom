#include "NullClass.h"
#include "NomClassType.h"
#include "RTClassType.h"
#include "NomType.h"
#include "ObjectHeader.h"
#include "NomJIT.h"

//const Nom::Runtime::NomNullClass *_NomNullClass= Nom::Runtime::NomNullClass::GetInstance();
////const Nom::Runtime::RTNullClass _RTNullClass;
////const Nom::Runtime::ObjectHeader NULLINSTANCE(&_RTNullClass, 0);
////const Nom::Runtime::ObjectHeader * const NULLOBJ = &NULLINSTANCE;
////const Nom::Runtime::RTClassType NULLCLASSTYPE(&_RTNullClass, nullptr);
////const Nom::Runtime::RTTypeHead NULLTYPE(&NULLCLASSTYPE);
//const Nom::Runtime::NomClassType NULLNOMCLASSTYPE(&_NomNullClass, Nom::Runtime::TypeList());
//extern "C" const Nom::Runtime::NomType * const NULLNOMTYPE = &NULLNOMCLASSTYPE;

namespace Nom
{
	namespace Runtime
	{

		//llvm::GlobalVariable * NomNullClass::NullObjectVar(llvm::Module & mod)
		//{
		//	static llvm::Module *mainModule = &mod;
		//	llvm::GlobalVariable *ret = mod.getGlobalVariable("NOM_NULLOBJ");
		//	if (ret == nullptr)
		//	{
		//		llvm::StructType *type = ObjectHeader::GetLLVMType(_NomNullClass.GetClassDescriptorType());
		//		ret = new llvm::GlobalVariable(mod, type, true, (&mod != mainModule ? (llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage) : (llvm::GlobalValue::LinkageTypes::ExternalLinkage)), nullptr, "NOM_NULLOBJ");
		//		ret->setInitializer(ObjectHeader::GetConstant(type, _NomNullClass.GetLLVMElement(mod)));
		//	}
		//	return ret;
		//}
		NomNullClass::NomNullClass() : NomInterface("Null_0"), NomClassInternal(new NomString("Null_0"))
			//NomClass(NomConstants::AddString(NomString("Null")), 0, 0, 0, nullptr)
		{
			this->SetDirectTypeParameters();
			//this->SetSuperClass();
			this->SetSuperInterfaces();
			this->compiled = true;
			this->preprocessed = true;
			//this->rtclass = &_RTNullClass;
		}
		NomNullClass * NomNullClass::GetInstance()
		{
			static NomNullClass nnc; return &nnc;
		}
		NomNullObject::NomNullObject()
		{
		}
		llvm::Constant * NomNullObject::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto cls = NomNullClass::GetInstance();
			//llvm::StructType *type = ObjectHeader::GetLLVMType(cls->GetLLVMElement(mod)->getType());
			return ObjectHeader::GetGlobal(mod, linkage, "RT_NOM_NULLOBJ", cls->GetLLVMElement(mod));;
		}
		llvm::Constant * NomNullObject::findLLVMElement(llvm::Module & mod) const
		{
			return ObjectHeader::FindGlobal(mod, "RT_NOM_NULLOBJ");
		}
		//llvm::GlobalVariable * NomNullClass::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		//{
		//	llvm::StructType *type = ObjectHeader::GetLLVMType(_NomNullClass.GetClassDescriptorType());
		//	ret = new llvm::GlobalVariable(mod, type, true, linkage, nullptr, "NOM_NULLOBJ");
		//	ret->setInitializer(ObjectHeader::GetConstant(type, _NomNullClass.GetLLVMElement(mod, refOnly)));
		//	return ret;
		//}
		//llvm::GlobalVariable * NomNullClass::findLLVMElement(llvm::Module & mod) const
		//{
		//	return mod.getGlobalVariable("NOM_NULLOBJ");
		//}
		void * NomNullClass::NullObject()
		{
			return (void*)(NomJIT::Instance().lookup("RT_NOM_NULLOBJ")->getAddress());
		}
	}
}