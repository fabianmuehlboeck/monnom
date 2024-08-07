#include "ArrayListClass.h"
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
#include "CompileHelpers.h"
#include "NomMethodTableEntry.h"
#include "RTConfig.h"
#include "llvm/Support/DynamicLibrary.h"
#include "IntClass.h"
#include "CastStats.h"
#include "NomClassType.h"
#include "IEnumerableInterface.h"
#include "IEnumeratorInterface.h"
#include "NomTypeParameter.h"
#include "BoehmContainers.h"
#include "BoolClass.h"


using namespace Nom::Runtime;
using namespace llvm;

typedef void* (*EnumeratorConstructorType)(void* arrlist, void* targ);
typedef BoehmVector<void*> ReferenceList;

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_GetEnumerator_0(void* arrlist)
{
	static EnumeratorConstructorType enumeratorConstructor = (EnumeratorConstructorType)GetGeneralLLVMFunction("RT_NOM_CCC_ArrayListEnumerator_1$$$CONSTRUCT$$$0$$CArrayList_1$VAR$$");
	void* typeArg = GetReadTypeArgFunction()(arrlist, 0);
	return enumeratorConstructor(typeArg, arrlist);
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_MoveNext_0(void* iter) noexcept(false)
{
	static auto writeField = GetWriteFieldFunction();
	static auto readField = GetReadFieldFunction();
	void* arraylist = readField(iter, 0);
	size_t version = (size_t)readField(iter, 1);
	size_t listversion = (size_t)readField(arraylist, 0);
	if (version != listversion)
	{
		throw new std::exception();
	}
	intptr_t* data = (intptr_t*)readField(iter, 3);
	size_t pos = (size_t)readField(iter, 4);
	size_t size = (size_t)readField(iter, 5);
	if (pos < size)
	{
		writeField(iter, 4, (void*)(pos + 1));
		writeField(iter, 2, (void*)data[pos]);
		return GetBooleanTrue();
	}
	//ReferenceList::iterator* current = (ReferenceList::iterator*)(((char*)arraylist) - (sizeof(intptr_t) * 3) - sizeof(ReferenceList::iterator));
	//ReferenceList::iterator* end = (ReferenceList::iterator*)(((char*)arraylist) - (sizeof(intptr_t) * 3) - (sizeof(ReferenceList::iterator) * 2));
	//if (*current != *end)
	//{
	//	writeField(iter, 2, *(*(current)));
	//	(*current)++;
	//	return GetBooleanTrue();
	//}
	writeField(iter, 2, nullptr);
	return GetBooleanFalse();
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_Current_0(void* iter) noexcept(false)
{
	static auto readField = GetReadFieldFunction();
	void* arraylist = readField(iter, 0);
	size_t version = (size_t)readField(iter, 1);
	size_t listversion = (size_t)readField(arraylist, 0);
	if (version != listversion)
	{
		throw new std::exception();
	}
	void* current = readField(iter, 2);
	if (current == nullptr)
	{
		throw new std::exception();
	}
	return current;
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Constructor_0(void* arraylist, void* targ)
{
	static auto writeTypeArg = GetWriteTypeArgFunction();
	static auto writeField = GetWriteFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	writeTypeArg(arraylist, 0, targ);
	writeField(arraylist, 0, (void*)1);
	void* valueptr = (void*)(fieldAddr(arraylist, 1));
	new(valueptr) ReferenceList();
	return arraylist;
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Length_0(void* arraylist)
{
	static auto fieldAddr = GetFieldAddrFunction();
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	return (void*)((intptr_t)lst->size());
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Add_1(void* arraylist, void* elem)
{
	static auto writeField = GetWriteFieldFunction();
	static auto readField = GetReadFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	lst->push_back(elem);
	writeField(arraylist, 0, (void*)(((size_t)readField(arraylist, 0)) + 1));
	return GetVoidObj();
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Contains_1(void* arraylist, void* elem)
{
	static auto readField = GetReadFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	auto iter = lst->begin();
	auto end = lst->end();
	while (iter != end)
	{
		void* val = *iter;
		if (((intptr_t)val) == ((intptr_t)elem))
		{
			return GetBooleanTrue();
		}
		iter++;
	}
	return GetBooleanFalse();
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Get_1(void* arraylist, void* index) noexcept(false)
{
	static auto readField = GetReadFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	uint64_t indexval = (uint64_t)index;
	if (indexval < 0 || indexval >= lst->size())
	{
		throw new std::exception();
	}
	return lst->at(indexval);
}

/*
These C++ functions get compiled to LLVM IR, packaged with the rest of the module, linked against rest of LLVM code. 
Runtime: metadata says there are specification foo, calls receiver (instance of class bar), calls its own method bas (call receiver.bass)
with arg1. Return result of that. 

Specification language that can interpret this. 

Constr calls and field access, casts, 

Make foo do callcheckedinstance method, runtime generates foo to do that. 


*/
extern "C" DLLEXPORT void* foo(void* receiver, void* arg1);


extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Set_2(void* arraylist, void* index, void* value) noexcept(false)
{
	static auto readField = GetReadFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	uint64_t indexval = (uint64_t)index;
	if (indexval < 0 || indexval >= lst->size())
	{
		throw new std::exception();
	}
	return lst->data()[indexval] = value;
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_Constructor_0(void* ArrayListEnumerator, void* targ, void* arraylist)
{
	static auto writeTypeArg = GetWriteTypeArgFunction();
	static auto writeField = GetWriteFieldFunction();
	static auto readField = GetReadFieldFunction();
	static auto fieldAddr = GetFieldAddrFunction();
	writeTypeArg(ArrayListEnumerator, 0, targ);
	writeField(ArrayListEnumerator, 0, arraylist);
	writeField(ArrayListEnumerator, 1, readField(arraylist, 0));
	writeField(ArrayListEnumerator, 2, nullptr);
	//ReferenceList::iterator* valueptr = (ReferenceList::iterator*)(((char*)arraylist) - (sizeof(intptr_t) * 3) - sizeof(ReferenceList::iterator));
	ReferenceList* lst = (ReferenceList*)(fieldAddr(arraylist, 1));
	writeField(ArrayListEnumerator, 3, lst->data());
	writeField(ArrayListEnumerator, 4, (void*)0);
	writeField(ArrayListEnumerator, 5, (void*)(lst->size()));
	//*valueptr = std::move(lst->begin());
	//valueptr = (ReferenceList::iterator*)(((char*)arraylist) - (sizeof(intptr_t) * 3) - (sizeof(ReferenceList::iterator) * 2));
	//*valueptr = std::move(lst->end());
	return ArrayListEnumerator;
}

namespace Nom
{
	namespace Runtime
	{
		ArrayListClass::ArrayListClass() : NomInterface("ArrayList_1"), NomClassInternal(new NomString("ArrayList_1"))
		{
			NomTypeParameterRef* ntparr = (NomTypeParameterRef*)nmalloc(sizeof(NomTypeParameterRef));
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 1));
			this->SetSuperClass();

			NomTypeRef ownarg = new NomTypeVar(ntparr[0]);
			NomTypeRef* ownargarr = new NomTypeRef[1]{ ownarg };
			NomTypeRef* intargarr = new NomTypeRef[2]{ NomIntClass::GetInstance()->GetType(), ownarg  };

			auto ienumerableInst = new NomInstantiationRef<NomInterface>(IEnumerableInterface::GetInstance(), TypeList(ownargarr,(size_t)1));
			auto ienumeratorType = IEnumeratorInterface::GetInstance()->GetType(TypeList(ownargarr, (size_t)1));

			this->SetSuperInterfaces(ArrayRef<NomInstantiationRef<NomInterface>>(ienumerableInst, 1));

			NomMethodInternal* getenum = new NomMethodInternal(this, "GetEnumerator", "LIB_NOM_ArrayList_GetEnumerator_0", true);
			getenum->SetDirectTypeParameters();
			getenum->SetArgumentTypes();
			getenum->SetReturnType(ienumeratorType);

			this->AddMethod(getenum);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_GetEnumerator_0", (void*)&LIB_NOM_ArrayList_GetEnumerator_0);

			NomMethodInternal* add = new NomMethodInternal(this, "Add", "LIB_NOM_ArrayList_Add_1", true);
			add->SetDirectTypeParameters();
			add->SetArgumentTypes(TypeList(ownargarr, 1));
			add->SetReturnType(NomVoidClass::GetInstance()->GetType());

			this->AddMethod(add);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Add_1", (void*)&LIB_NOM_ArrayList_Add_1);

			NomMethodInternal* contains = new NomMethodInternal(this, "Contains", "LIB_NOM_ArrayList_Contains_1", true);
			contains->SetDirectTypeParameters();
			contains->SetArgumentTypes(TypeList(ownargarr, 1));
			contains->SetReturnType(NomBoolClass::GetInstance()->GetType());

			this->AddMethod(contains);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Contains_1", (void*)&LIB_NOM_ArrayList_Contains_1);

			NomMethodInternal* set = new NomMethodInternal(this, "Set", "LIB_NOM_ArrayList_Set_2", true);
			set->SetDirectTypeParameters();
			set->SetArgumentTypes(TypeList(intargarr, 2));
			set->SetReturnType(NomVoidClass::GetInstance()->GetType());

			this->AddMethod(set);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Set_2", (void*)&LIB_NOM_ArrayList_Set_2);

			NomMethodInternal* get = new NomMethodInternal(this, "Get", "LIB_NOM_ArrayList_Get_1", true);
			get->SetDirectTypeParameters();
			get->SetArgumentTypes(TypeList(intargarr, 1));
			get->SetReturnType(ownarg);

			this->AddMethod(get);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Get_1", (void*)&LIB_NOM_ArrayList_Get_1);

			NomMethodInternal* len = new NomMethodInternal(this, "Length", "LIB_NOM_ArrayList_Length_0", true);
			len->SetDirectTypeParameters();
			len->SetArgumentTypes();
			len->SetReturnType(NomIntClass::GetInstance()->GetType());

			this->AddMethod(len);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Length_0", (void*)&LIB_NOM_ArrayList_Length_0);

			NomConstructorInternal* constructor = new NomConstructorInternal("LIB_NOM_ArrayList_Constructor_0", this);
			constructor->SetDirectTypeParameters();
			constructor->SetArgumentTypes();

			this->AddConstructor(constructor);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayList_Constructor_0", (void*)&LIB_NOM_ArrayList_Constructor_0);
		}

		ArrayListClass* ArrayListClass::GetInstance() {
			static ArrayListClass nsc;
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
		size_t ArrayListClass::GetFieldCount() const
		{
			auto size = sizeof(ReferenceList);
			size_t fields=1;
			while (size > 0)
			{
				fields++;
				size -= sizeof(intptr_t);
			}
			return fields;
		}

		ArrayListEnumeratorClass::ArrayListEnumeratorClass() : NomInterface("ArrayListEnumerator_1"), NomClassInternal(new NomString("ArrayListEnumerator_1"))
		{
			NomTypeParameterRef* ntparr = (NomTypeParameterRef*)nmalloc(sizeof(NomTypeParameterRef));
			ntparr[0] = new NomTypeParameterInternal(this, 0, NomType::AnythingRef, NomType::NothingRef);
			SetDirectTypeParameters(llvm::ArrayRef<NomTypeParameterRef>(ntparr, 1));
			this->SetSuperClass();

			NomTypeRef ownarg = new NomTypeVar(ntparr[0]);
			NomTypeRef* ownargarr = new NomTypeRef[1]{ ownarg };

			auto ienumeratorInst = new NomInstantiationRef<NomInterface>(IEnumeratorInterface::GetInstance(), TypeList(ownargarr,1));
			//auto ienumeratorType = IEnumeratorInterface::GetInstance()->GetType({ ownarg });

			this->SetSuperInterfaces(ArrayRef<NomInstantiationRef<NomInterface>>(ienumeratorInst, 1));

			NomMethodInternal* movenext = new NomMethodInternal(this, "MoveNext", "LIB_NOM_ArrayListEnumerator_MoveNext_0", true);
			movenext->SetDirectTypeParameters();
			movenext->SetArgumentTypes();
			movenext->SetReturnType(NomBoolClass::GetInstance()->GetType());

			this->AddMethod(movenext);
			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayListEnumerator_MoveNext_0", (void*)&LIB_NOM_ArrayListEnumerator_MoveNext_0);

			NomMethodInternal* current = new NomMethodInternal(this, "Current", "LIB_NOM_ArrayListEnumerator_Current_0", true);
			current->SetDirectTypeParameters();
			current->SetArgumentTypes();
			current->SetReturnType(ownarg);
			this->AddMethod(current);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayListEnumerator_Current_0", (void*)&LIB_NOM_ArrayListEnumerator_Current_0);

			NomTypeRef* arrlisttypelist = new NomTypeRef[1]{ArrayListClass::GetInstance()->GetType(TypeList(ownargarr,1))};

			NomConstructorInternal* constructor = new NomConstructorInternal("LIB_NOM_ArrayListEnumerator_Constructor_0", this);
			constructor->SetDirectTypeParameters();
			constructor->SetArgumentTypes(TypeList(arrlisttypelist, 1));

			this->AddConstructor(constructor);

			llvm::sys::DynamicLibrary::AddSymbol("LIB_NOM_ArrayListEnumerator_Constructor_0", (void*)&LIB_NOM_ArrayListEnumerator_Constructor_0);
		}
		ArrayListEnumeratorClass* ArrayListEnumeratorClass::GetInstance()
		{
			static ArrayListEnumeratorClass nsc;
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
		size_t ArrayListEnumeratorClass::GetFieldCount() const
		{
			return 6; // ArrayList, Version, Current, Underlying Array, Current Pos, Size
			//auto size = sizeof(ReferenceList::iterator);
			//size_t fields = 3;
			//while (size > 0)
			//{
			//	fields++;
			//	fields++;
			//	size -= sizeof(intptr_t);
			//}
			//return fields;
		}
		void ArrayListEnumeratorClass::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
			results.push_back(IEnumeratorInterface::GetInstance());
		}
		void ArrayListClass::GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4> &results) const
		{
			results.push_back(ArrayListEnumeratorClass::GetInstance());
		}
		void ArrayListClass::GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const
		{
			results.push_back(IEnumerableInterface::GetInstance());
		}
	}
}