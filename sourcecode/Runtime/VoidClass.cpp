#include "VoidClass.h"
#include "NomClassType.h"
#include "NomJIT.h"

//Nom::Runtime::NomVoidClass *_NomVoidClass= Nom::Runtime::NomVoidClass::GetInstance();
////const Nom::Runtime::RTVoidClass _RTVoidClass;
////const Nom::Runtime::ObjectHeader VOIDINSTANCE(&_RTVoidClass, 0);
////const Nom::Runtime::ObjectHeader * const VOIDOBJ = &VOIDINSTANCE;
////const Nom::Runtime::RTClassType VOIDCLASSTYPE(&_RTVoidClass, nullptr);
////const Nom::Runtime::RTTypeHead VOIDTYPE(&VOIDCLASSTYPE);
//const Nom::Runtime::NomClassType VOIDNOMCLASSTYPE(&_NomVoidClass, Nom::Runtime::TypeList());
//extern "C" const Nom::Runtime::NomType * const VOIDNOMTYPE = &VOIDNOMCLASSTYPE;

namespace Nom
{
	namespace Runtime
	{
		NomVoidClass::NomVoidClass() : NomInterface("Void_0"), NomClassInternal(new NomString("Void_0"))
			//NomClass(NomConstants::AddString(NomString("Void")), 0, 0, 0, nullptr)
		{
			this->SetDirectTypeParameters();
			//this->SetSuperClass();
			this->SetSuperInterfaces();
			//compiled = true;
			preprocessed = true;
		}


		NomVoidClass::~NomVoidClass()
		{
		}

		NomVoidClass* NomVoidClass::GetInstance() {
			static NomVoidClass nvc; 
			return &nvc; 
		}

		//llvm::GlobalVariable * NomVoidClass::VoidObjectVar(llvm::Module & mod)
		//{
		//	static llvm::Module *mainModule = &mod;
		//	llvm::GlobalVariable *ret = mod.getGlobalVariable("NOM_VOIDOBJ");
		//	if (ret == nullptr)
		//	{
		//		llvm::StructType *type = ObjectHeader::GetLLVMType(_NomVoidClass.GetClassDescriptorType());
		//		ret = new llvm::GlobalVariable(mod, type, true, (&mod!=mainModule ? (llvm::GlobalValue::LinkageTypes::AvailableExternallyLinkage) : (llvm::GlobalValue::LinkageTypes::ExternalLinkage)), nullptr, "NOM_VOIDOBJ");
		//		ret->setInitializer(ObjectHeader::GetConstant(type, _NomVoidClass.GetLLVMElement(mod)));
		//	}
		//	return ret;
		//}
		void* NomVoidClass::VoidObject()
		{
			return (void*)(NomJIT::Instance().lookup("RT_NOM_VOIDOBJ")->getAddress());
		}
		NomVoidObject* NomVoidObject::GetInstance()
		{
			static NomVoidObject nvo;
			return &nvo;
		}
		llvm::Constant* NomVoidObject::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto cls = NomVoidClass::GetInstance();
			llvm::StructType* type = ObjectHeader::GetLLVMType(/*cls->GetLLVMElement(mod)->getType()*/);
			return ObjectHeader::GetGlobal(mod, linkage, "RT_NOM_VOIDOBJ", cls->GetLLVMElement(mod));;
		}
		llvm::Constant* NomVoidObject::findLLVMElement(llvm::Module& mod) const
		{
			return ObjectHeader::FindGlobal(mod, "RT_NOM_VOIDOBJ");
			//return mod.getGlobalVariable("RT_NOM_VOIDOBJ");
		}
	}
}
