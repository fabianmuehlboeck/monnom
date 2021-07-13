#include "ObjectClass.h"
#include "NomConstants.h"
#include "NomClassType.h"
#include "TypeList.h"
#include "NomString.h"
#include "DLLExport.h"
#include "llvm/Support/DynamicLibrary.h"

//Nom::Runtime::NomObjectClass *_NomObjectClass = Nom::Runtime::NomObjectClass::GetInstance();
////const Nom::Runtime::RTObjectClass _RTObjectClass;
//const Nom::Runtime::NomClassType _NomObjectClassType(&_NomObjectClass, Nom::Runtime::TypeList());
//const Nom::Runtime::NomClassType * _NomObjectClassTypeRef = &_NomObjectClassType;
////const Nom::Runtime::RTClassType _RTObjectClassType(&_RTObjectClass, sizeof(_RTObjectClass));

extern "C" DLLEXPORT void* LIB_NOM_Object_Constructor_0(void* obj)
{
	return obj;
}
namespace Nom
{
	namespace Runtime
	{
		NomObjectClass::NomObjectClass() : NomInterface("Object_0"), NomClassInternal(new NomString("Object_0"))
			//NomClass(NomConstants::AddString(NomString("Object")), 0, 0, 0, nullptr)
		{
			//this->compiled = true;
			this->preprocessed = true;
			////this->rtclass = &_RTObjectClass;
			//this->AddConstructor(0, 1);
			this->SetDirectTypeParameters();
			this->SetSuperInterfaces();

			NomConstructorInternal* nci = new NomConstructorInternal("LIB_NOM_Object_Constructor_0", this);
			nci->SetDirectTypeParameters();
			nci->SetArgumentTypes();
			this->AddConstructor(nci);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Object_Constructor_0", (void*)&LIB_NOM_Object_Constructor_0);
		}

		NomObjectClass * NomObjectClass::GetInstance() { static NomObjectClass noc; return &noc; }
	}
}
