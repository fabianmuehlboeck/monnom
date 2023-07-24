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
PUSHDIAGSUPPRESSION
#include "llvm/Support/DynamicLibrary.h"
POPDIAGSUPPRESSION
#include "NomClassType.h"
#include "IComparableInterface.h"

namespace Nom
{
	namespace Runtime
	{
		NomIntClass::NomIntClass() : NomInterface(), NomClassInternal(new NomString("Int_0"))
		{
			this->SetDirectTypeParameters();
			this->SetSuperClass();

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Int_ToString_1", reinterpret_cast<void*>(& LIB_NOM_Int_ToString_1));
		}


		NomIntClass* NomIntClass::GetInstance() {
			[[clang::no_destroy]] static NomIntClass nic;
			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
				NomStringClass::GetInstance();

				NomMethodInternal* toString = new NomMethodInternal(&nic, "ToString", "LIB_NOM_Int_ToString_1", true);
				toString->SetDirectTypeParameters();
				toString->SetArgumentTypes();
				toString->SetReturnType(NomStringClass::GetInstance()->GetType());
				nic.AddMethod(toString);


				NomTypeRef* intTypeArr = makenmalloc(NomTypeRef, 1);
				intTypeArr[0] = nic.GetType();

				NomInstantiationRef<NomInterface>* superInterfacesArr = makenmalloc(NomInstantiationRef<NomInterface>, 1);
				superInterfacesArr[0] = NomInstantiationRef<NomInterface>(IComparableInterface::GetInstance(), TypeList(intTypeArr,1));

				nic.SetSuperInterfaces(llvm::ArrayRef<NomInstantiationRef<NomInterface>>(superInterfacesArr,1));


				NomMethodInternal* compare = new NomMethodInternal(&nic, "Compare", "LIB_NOM_Int_Compare_1", true);
				compare->SetDirectTypeParameters();


				compare->SetArgumentTypes(TypeList(intTypeArr,1));
				compare->SetReturnType(nic.GetType());
				nic.AddMethod(compare);
			}
			return &nic;
		}

		NomIntClass::~NomIntClass()
		{
		}

		llvm::Constant* NomIntClass::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			NomStringClass::GetInstance()->GetLLVMElement(mod);
			return NomClass::createLLVMElement(mod, linkage);
		}
	}
}
extern "C" DLLEXPORT const void* LIB_NOM_Int_ToString_1(const int64_t value)
{
	char buf[64];
	snprintf(buf, 64, "%" PRIiPTR "", value);
	Nom::Runtime::NomString* nomstring = new (Nom::Runtime::gcalloc_atomic(sizeof(Nom::Runtime::NomString))) Nom::Runtime::NomString(buf);
	return nomstring->GetStringObject();
}

extern "C" DLLEXPORT int64_t LIB_NOM_Int_Compare_1(const int64_t value, const int64_t other)
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
