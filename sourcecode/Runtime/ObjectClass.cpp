#include "ObjectClass.h"
#include "NomConstants.h"
#include "NomClassType.h"
#include "NomString.h"
#include "DLLExport.h"
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION

extern "C" DLLEXPORT void* LIB_NOM_Object_Constructor_0(void* obj)
{
	return obj;
}
namespace Nom
{
	namespace Runtime
	{
		NomObjectClass::NomObjectClass() : NomInterface(), NomClassInternal(new NomString("Object_0"))
		{
			this->preprocessed = true;
			this->SetDirectTypeParameters();
			this->SetSuperInterfaces();

			NomConstructorInternal* nci = new NomConstructorInternal("LIB_NOM_Object_Constructor_0", this);
			nci->SetDirectTypeParameters();
			nci->SetArgumentTypes();
			this->AddConstructor(nci);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Object_Constructor_0", reinterpret_cast<void*>(& LIB_NOM_Object_Constructor_0));
		}

		NomObjectClass * NomObjectClass::GetInstance() { [[clang::no_destroy]] static NomObjectClass noc; return &noc; }
	}
}
