#include "FloatClass.h"
#include "IntClass.h"
#include "ObjectClass.h"
#include "NomAlloc.h"
#include "BoehmAtomicAllocator.h"
#include <cstdio>
#include <limits>
#include <cstdint>
#include <cinttypes>
#include "Defs.h"
#include <iostream>
#include "NomMethodTableEntry.h"
#include "StringClass.h"
#include "llvm/Support/DynamicLibrary.h"
#include "NomClassType.h"
#include "IComparableInterface.h"

//Nom::Runtime::NomFloatClass *_NomFloatClass=Nom::Runtime::NomFloatClass::GetInstance();
////const Nom::Runtime::RTFloatClass _RTFloatClass;
//const Nom::Runtime::NomFloatClass * const _NomFloatClassRef = &_NomFloatClass;
////const Nom::Runtime::RTFloatClass * const _RTFloatClassRef = &_RTFloatClass;
//const Nom::Runtime::NomClass * const _NomFloatClassNC = &_NomFloatClass;
////const Nom::Runtime::RTClass * const _RTFloatClassRTC = &_RTFloatClass;

extern "C" DLLEXPORT const void* LIB_NOM_Float_ToString_1(const double value)
{
	char buf[64];
	//std::cout << "\n";
	snprintf(buf, 64, "%f", value);
	//Nom::Runtime::NomChar ncbuf[64];
	//for (size_t i = strlen(buf); i > 0;)
	//{
	//	i--;
	//	ncbuf[i] = buf[i];
	//}
	Nom::Runtime::NomString* nomstring = new (Nom::Runtime::gcalloc_atomic(sizeof(Nom::Runtime::NomString))) Nom::Runtime::NomString(buf);
	//std::cout << ": ";
	//std::cout << std::hex << (intptr_t)nomstring;
	//std::cout << "\n";
	return nomstring->GetStringObject();
	//return Nom::Runtime::NomString::GetStringObject(strlen(buf)*sizeof(Nom::Runtime::NomChar), ncbuf);
	//Nom::Runtime::ObjectHeader obj = CPP_NOM_CreateInstance(_RTStringClassRTC, 0, nullptr, nullptr);
	//obj.Fields() = (intptr_t)nomstring;
	//return obj;
}

extern "C" DLLEXPORT const int64_t LIB_NOM_Float_Compare_1(const double value, const double other)
{
	if (value == other)
	{
		return 0;
	}
	else if (value < other)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

namespace Nom
{
	namespace Runtime
	{
		NomFloatClass::NomFloatClass() : NomInterface("Float_0"), NomClassInternal(new NomString("Float_0"))
			//NomClass(NomConstants::AddString(NomString("Float")), 0, NomConstants::AddSuperClass(NomConstants::AddClass(NomConstants::AddString(NomString("stdlib")), NomConstants::AddString(NomString("Object"))), 0), 0, nullptr)
		{
			SetDirectTypeParameters();
			SetSuperClass(NomInstantiationRef<NomClass>(NomObjectClass::GetInstance(), TypeList()));
			//SetSuperInterfaces();
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Float_ToString_1", (void*)&LIB_NOM_Float_ToString_1);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Float_Compare_1", (void*)&LIB_NOM_Float_Compare_1);
		}


		NomFloatClass* NomFloatClass::GetInstance() {
			static NomFloatClass nfc;

			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
				NomStringClass::GetInstance();
				NomIntClass::GetInstance();

				NomMethodInternal* toString = new NomMethodInternal(&nfc, "ToString", "LIB_NOM_Float_ToString_1", true);
				toString->SetDirectTypeParameters();
				toString->SetArgumentTypes();
				toString->SetReturnType(NomStringClass::GetInstance()->GetType());
				nfc.AddMethod(toString);


				NomTypeRef* floatTypeArr = makenmalloc(NomTypeRef, 1);
				floatTypeArr[0] = nfc.GetType();

				NomInstantiationRef<NomInterface>* superInterfacesArr = makenmalloc(NomInstantiationRef<NomInterface>, 1);
				superInterfacesArr[0] = NomInstantiationRef<NomInterface>(IComparableInterface::GetInstance(), TypeList(floatTypeArr, 1));

				nfc.SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>>(superInterfacesArr, 1));


				NomMethodInternal* compare = new NomMethodInternal(&nfc, "Compare", "LIB_NOM_Float_Compare_1", true);
				compare->SetDirectTypeParameters();


				compare->SetArgumentTypes(TypeList(floatTypeArr, 1));
				compare->SetReturnType(NomIntClass::GetInstance()->GetType());
				nfc.AddMethod(compare);

				//nfc.PreprocessInheritance();
			}
			return &nfc;

		}

		NomFloatClass::~NomFloatClass()
		{
		}

		//RTFloatClass::RTFloatClass() : RTClass(0, 0)
		//{

		//}
	}
}