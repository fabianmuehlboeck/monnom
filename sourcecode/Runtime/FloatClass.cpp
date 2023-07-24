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
#include "CompileHelpers.h"
#include "RefValueHeader.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GCC__)
#pragma GCC diagnostic ignored "-Wfloat-equal"
#elif defined(_MSC_VER)

#endif


extern "C" DLLEXPORT const void* LIB_NOM_Float_ToString_1(const double value)
{
	char buf[64];
	snprintf(buf, 64, "%f", value);
	Nom::Runtime::NomString* nomstring = new (Nom::Runtime::gcalloc_atomic(sizeof(Nom::Runtime::NomString))) Nom::Runtime::NomString(buf);
	return nomstring->GetStringObject();
}

extern "C" DLLEXPORT int64_t LIB_NOM_Float_Compare_1(const double value, const double other)
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
		NomFloatClass::NomFloatClass() : NomInterface(), NomClassInternal(new NomString("Float_0"))
		{
			SetDirectTypeParameters();
			SetSuperClass(NomInstantiationRef<NomClass>(NomObjectClass::GetInstance(), TypeList()));
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Float_ToString_1", reinterpret_cast<void*>(& LIB_NOM_Float_ToString_1));
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Float_Compare_1", reinterpret_cast<void*>(&LIB_NOM_Float_Compare_1));
		}


		NomFloatClass* NomFloatClass::GetInstance() {
			[[clang::no_destroy]] static NomFloatClass nfc;

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
			}
			return &nfc;

		}

		NomFloatClass::~NomFloatClass()
		{
		}


		NomFloatObjects::NomFloatObjects()
		{
		}

		llvm::Constant* NomFloatObjects::GetPosZero(llvm::Module& mod)
		{
			auto elem = GetInstance()->GetLLVMElement(mod);
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), elem, llvm::ArrayRef<llvm::Constant*>({{MakeInt32(0), MakeInt32(0), MakeInt32(ObjectHeaderFields::RefValueHeader)}})), REFTYPE);
		}

		llvm::Constant* NomFloatObjects::GetNegZero(llvm::Module& mod)
		{
			auto elem = GetInstance()->GetLLVMElement(mod);
			return llvm::ConstantExpr::getPointerCast(llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), elem, llvm::ArrayRef<llvm::Constant*>({ {MakeInt32(0), MakeInt32(1), MakeInt32(ObjectHeaderFields::RefValueHeader)} })), REFTYPE);
		}


		llvm::Constant* NomFloatObjects::createLLVMElement(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage) const
		{
			auto var = new llvm::GlobalVariable(mod, arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), true, linkage, nullptr, "RT_NOM_FLOATS");
			auto clsref = NomFloatClass::GetInstance()->GetLLVMElement(mod);
			auto fieldstype = arrtype(REFTYPE, 1);
			llvm::Constant* posZeroConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(ConstantExpr::getBitCast(ConstantFP::get(FLOATTYPE,0.0), numtype(intptr_t)), REFTYPE) }), llvm::ConstantArray::get(arrtype(RTTypeHead::GetLLVMType()->getPointerTo(), 0), {}));
			llvm::Constant* negZeroConst = ObjectHeader::GetConstant(clsref, llvm::ConstantArray::get(fieldstype, { llvm::ConstantExpr::getIntToPtr(ConstantExpr::getBitCast(ConstantFP::getNegativeZero(FLOATTYPE), numtype(intptr_t)), REFTYPE) }), llvm::ConstantArray::get(arrtype(RTTypeHead::GetLLVMType()->getPointerTo(), 0), {}));
			llvm::Constant* arr = llvm::ConstantArray::get(arrtype(posZeroConst->getType(), 2), { {posZeroConst, negZeroConst} });
			var->setAlignment(llvm::MaybeAlign(8));
			var->setInitializer(arr);

			new llvm::GlobalVariable(mod, RefValueHeader::GetLLVMType()->getPointerTo(), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(0),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_POSZERO");
			new llvm::GlobalVariable(mod, RefValueHeader::GetLLVMType()->getPointerTo(), true, linkage, llvm::ConstantExpr::getGetElementPtr(arrtype(ObjectHeader::GetLLVMType(1, 0, false), 2), var, llvm::ArrayRef<llvm::Constant*>({ MakeInt32(0), MakeInt32(1),MakeInt32(ObjectHeaderFields::RefValueHeader) })), "RT_NOM_NEGZERO");
			return var;
		}

		llvm::Constant* NomFloatObjects::findLLVMElement(llvm::Module& mod) const
		{
			return mod.getGlobalVariable("RT_NOM_FLOATS");
		}
	}
}
