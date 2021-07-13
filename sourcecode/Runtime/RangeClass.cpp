#include "RangeClass.h"
#include "ObjectClass.h"
#include "NomAlloc.h"
#include <iostream>
#include "Defs.h"
#include "NomMethod.h"
#include "NullClass.h"
#include "NomConstants.h"
#include "ObjectHeader.h"
#include "VoidClass.h"
#include "NomJIT.h"
#include "NomMethodTableEntry.h"
#include "RTConfig.h"
#include "llvm/Support/DynamicLibrary.h"
#include "IntClass.h"
#include "CastStats.h"
#include "NomClassType.h"
#include "IEnumerableInterface.h"
#include "IEnumeratorInterface.h"
#include "NomTypeParameter.h"
#include "BoolClass.h"
#include "CompileHelpers.h"

using namespace Nom::Runtime;

typedef void* (*EnumeratorConstructorType)(intptr_t start, intptr_t end, intptr_t step);

extern "C" DLLEXPORT void* LIB_NOM_Range_GetEnumerator_0(void* range)
{
	static EnumeratorConstructorType enumeratorConstructor = (EnumeratorConstructorType)GetGeneralLLVMFunction("RT_NOM_CCC_RangeEnumerator_0$$$CONSTRUCT$$$0$$CInt_0$$$$.$CInt_0$$$$.$CInt_0$$$");
	static auto readField = GetReadFieldFunction();
	intptr_t start = (intptr_t)readField(range, 0);
	intptr_t end = (intptr_t)readField(range, 1);
	intptr_t step = (intptr_t)readField(range, 2);
	return enumeratorConstructor(start, end, step);
}

extern "C" DLLEXPORT void* LIB_NOM_RangeEnumerator_MoveNext_0(void* iter) noexcept(false)
{
	static auto writeField = GetWriteFieldFunction();
	static auto readField = GetReadFieldFunction();
	intptr_t state = (intptr_t)readField(iter, 3);
	if (state == 2)
	{
		return GetBooleanFalse();
	}
	intptr_t start = (intptr_t)readField(iter, 0);
	intptr_t end = (intptr_t)readField(iter, 1);
	intptr_t step = (intptr_t)readField(iter, 2);
	if (state == 0)
	{
		if ((step > 0 && start >= end) || (step < 0 && start <= end) || step == 0)
		{
			writeField(iter, 3, (void*)2);
			return GetBooleanFalse();
		}
		writeField(iter, 3, (void*)1);
		return GetBooleanTrue();
	}
	start += step;
	if ((step > 0 && start >= end) || (step < 0 && start <= end))
	{
		writeField(iter, 3, (void*)2);
		return GetBooleanFalse();
	}
	writeField(iter, 0, (void*)start);
	return GetBooleanTrue();
}

extern "C" DLLEXPORT void* LIB_NOM_RangeEnumerator_Current_0(void* iter) noexcept(false)
{
	static auto readField = GetReadFieldFunction();
	intptr_t start = (intptr_t)readField(iter, 0);
	intptr_t state = (intptr_t)readField(iter, 3);
	if(state!=1)
	{
		throw new std::exception();
	}
	return (void*)start;
}

extern "C" DLLEXPORT void* LIB_NOM_Range_Constructor_3(void* rng, intptr_t start, intptr_t end, intptr_t step)
{
	static auto writeField = GetWriteFieldFunction();
	writeField(rng, 0, (void*)start);
	writeField(rng, 1, (void*)end);
	writeField(rng, 2, (void*)step);
	return rng;
}

extern "C" DLLEXPORT void* LIB_NOM_RangeEnumerator_Constructor_3(void* RangeEnumerator, intptr_t start, intptr_t end, intptr_t step)
{
	static auto writeField = GetWriteFieldFunction();
	writeField(RangeEnumerator, 0, (void*)start);
	writeField(RangeEnumerator, 1, (void*)end);
	writeField(RangeEnumerator, 2, (void*)step);
	writeField(RangeEnumerator, 3, (void*)0);
	return RangeEnumerator;
}

namespace Nom
{
	namespace Runtime
	{
		RangeClass::RangeClass() : NomInterface("Range_0"), NomClassInternal(new NomString("Range_0"))
		{
			SetDirectTypeParameters();
			this->SetSuperClass();

			NomTypeRef* intargarr = new NomTypeRef[1]{ NomIntClass::GetInstance()->GetType() };

			auto ienumerableInst = new NomInstantiationRef<NomInterface>(IEnumerableInterface::GetInstance(), TypeList(intargarr, (size_t)1));
			auto ienumeratorType = IEnumeratorInterface::GetInstance()->GetType(TypeList(intargarr, (size_t)1));

			this->SetSuperInterfaces(ArrayRef<NomInstantiationRef<NomInterface>>(ienumerableInst, 1));

			NomMethodInternal* getenum = new NomMethodInternal(this, "GetEnumerator", "LIB_NOM_Range_GetEnumerator_0", true);
			getenum->SetDirectTypeParameters();
			getenum->SetArgumentTypes();
			getenum->SetReturnType(ienumeratorType);

			this->AddMethod(getenum);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Range_GetEnumerator_0", (void*)&LIB_NOM_Range_GetEnumerator_0);

			NomConstructorInternal* constructor = new NomConstructorInternal("LIB_NOM_Range_Constructor_3", this);
			constructor->SetDirectTypeParameters();

			NomTypeRef* constArgArr = new NomTypeRef[3]{ NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType() };

			constructor->SetArgumentTypes(TypeList(constArgArr, 3));

			this->AddConstructor(constructor);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_Range_Constructor_3", (void*)&LIB_NOM_Range_Constructor_3);
		}

		RangeClass* RangeClass::GetInstance() {
			static RangeClass nsc;
			static bool once = true;
			if (once)
			{
				once = false;
				NomObjectClass::GetInstance();
				NomIntClass::GetInstance();
				NomVoidClass::GetInstance();
				//nsc.PreprocessInheritance();
			}
			return &nsc;
		}
		size_t RangeClass::GetFieldCount() const
		{
			return 3;
		}

		RangeEnumeratorClass::RangeEnumeratorClass() : NomInterface("RangeEnumerator_0"), NomClassInternal(new NomString("RangeEnumerator_0"))
		{
			SetDirectTypeParameters();
			this->SetSuperClass();


			NomTypeRef* intargarr = new NomTypeRef[1]{ NomIntClass::GetInstance()->GetType() };

			auto ienumeratorInst = new NomInstantiationRef<NomInterface>(IEnumeratorInterface::GetInstance(), TypeList(intargarr, 1));
			//auto ienumeratorType = IEnumeratorInterface::GetInstance()->GetType({ ownarg });

			this->SetSuperInterfaces(ArrayRef<NomInstantiationRef<NomInterface>>(ienumeratorInst, 1));

			NomMethodInternal* movenext = new NomMethodInternal(this, "MoveNext", "LIB_NOM_RangeEnumerator_MoveNext_0", true);
			movenext->SetDirectTypeParameters();
			movenext->SetArgumentTypes();
			movenext->SetReturnType(NomBoolClass::GetInstance()->GetType());

			this->AddMethod(movenext);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_RangeEnumerator_MoveNext_0", (void*)&LIB_NOM_RangeEnumerator_MoveNext_0);

			NomMethodInternal* current = new NomMethodInternal(this, "Current", "LIB_NOM_RangeEnumerator_Current_0", true);
			current->SetDirectTypeParameters();
			current->SetArgumentTypes();
			current->SetReturnType(NomIntClass::GetInstance()->GetType());
			this->AddMethod(current);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_RangeEnumerator_Current_0", (void*)&LIB_NOM_RangeEnumerator_Current_0);

			NomConstructorInternal* constructor = new NomConstructorInternal("LIB_NOM_RangeEnumerator_Constructor_3", this);
			constructor->SetDirectTypeParameters();

			NomTypeRef* constArgArr = new NomTypeRef[3]{ NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType(), NomIntClass::GetInstance()->GetType() };

			constructor->SetArgumentTypes(TypeList(constArgArr, 3));

			this->AddConstructor(constructor);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_RangeEnumerator_Constructor_3", (void*)&LIB_NOM_RangeEnumerator_Constructor_3);
		}
		RangeEnumeratorClass* RangeEnumeratorClass::GetInstance()
		{
			static RangeEnumeratorClass nsc;
			static bool once = true;
			if (once)
			{
				once = false;
				IEnumeratorInterface::GetInstance();
				NomObjectClass::GetInstance();
				NomIntClass::GetInstance();
				NomVoidClass::GetInstance();
				//nsc.PreprocessInheritance();
			}
			return &nsc;
		}
		size_t RangeEnumeratorClass::GetFieldCount() const
		{
			return 4; // Start, End, Step, State (0 = Initial, 1 = Running, 2 = Stopped)
		}
		void RangeEnumeratorClass::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
			results.push_back(IEnumeratorInterface::GetInstance());
		}
		void RangeClass::GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const
		{
			results.push_back(RangeEnumeratorClass::GetInstance());
		}
		void RangeClass::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
			results.push_back(IEnumerableInterface::GetInstance());
		}
	}
}