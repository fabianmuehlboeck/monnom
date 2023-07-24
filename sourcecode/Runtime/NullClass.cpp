#include "NullClass.h"
#include "NomClassType.h"
#include "RTClassType.h"
#include "NomType.h"
#include "ObjectHeader.h"
#include "NomJIT.h"

namespace Nom
{
	namespace Runtime
	{

		NomNullClass::NomNullClass() : NomInterface(), NomClassInternal(new NomString("Null_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperInterfaces();
			this->compiled = true;
			this->preprocessed = true;
		}
		NomNullClass * NomNullClass::GetInstance()
		{
			[[clang::no_destroy]] static NomNullClass nnc; return &nnc;
		}
		NomNullObject::NomNullObject()
		{
		}
		llvm::Constant * NomNullObject::createLLVMElement(llvm::Module & mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto cls = NomNullClass::GetInstance();
			return ObjectHeader::GetGlobal(mod, linkage, "RT_NOM_NULLOBJ", cls->GetLLVMElement(mod));
		}
		llvm::Constant * NomNullObject::findLLVMElement(llvm::Module & mod) const
		{
			return ObjectHeader::FindGlobal(mod, "RT_NOM_NULLOBJ");
		}
		void * NomNullClass::NullObject()
		{
			return reinterpret_cast<void*>(NomJIT::Instance().lookup("RT_NOM_NULLOBJ")->getValue());
		}
	}
}
