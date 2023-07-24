#include "VoidClass.h"
#include "NomClassType.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{
		NomVoidClass::NomVoidClass() : NomInterface(), NomClassInternal(new NomString("Void_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperClass();
			this->SetSuperInterfaces();
		}


		NomVoidClass::~NomVoidClass()
		{
		}

		NomVoidClass* NomVoidClass::GetInstance() {
			[[clang::no_destroy]] static NomVoidClass nvc; 
			return &nvc; 
		}

		void* NomVoidClass::VoidObject()
		{
			return reinterpret_cast<void*>(NomJIT::Instance().lookup("RT_NOM_VOIDOBJ")->getValue());
		}
		NomVoidObject* NomVoidObject::GetInstance()
		{
			[[clang::no_destroy]] static NomVoidObject nvo;
			return &nvo;
		}
		llvm::Constant* NomVoidObject::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto cls = NomVoidClass::GetInstance();
			llvm::StructType* type = ObjectHeader::GetLLVMType();
			return ObjectHeader::GetGlobal(mod, linkage, "RT_NOM_VOIDOBJ", cls->GetLLVMElement(mod));
		}
		llvm::Constant* NomVoidObject::findLLVMElement(llvm::Module& mod) const
		{
			return ObjectHeader::FindGlobal(mod, "RT_NOM_VOIDOBJ");
		}
	}
}
