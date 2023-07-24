#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "NomVMInterface.h"

namespace Nom
{
	namespace Runtime
	{
		class ObjectHeader;
		class ArrayListClass : public NomClassInternal
		{
		private:
			ArrayListClass();
		public:
			static ArrayListClass* GetInstance();
			virtual ~ArrayListClass() override {}
			virtual size_t GetFieldCount() const override;
			virtual void GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const override;
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;
		};

		class ArrayListEnumeratorClass : public NomClassInternal
		{
		private:
			ArrayListEnumeratorClass();
		public:
			static ArrayListEnumeratorClass* GetInstance();
			virtual ~ArrayListEnumeratorClass() override {}
			virtual size_t GetFieldCount() const override;

			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;
		};
	}
}

extern "C" DLLEXPORT void* LIB_NOM_ArrayList_GetEnumerator_0(void* arrlist);
extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_MoveNext_0(void* iter) noexcept(false);
extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_Current_0(void* iter) noexcept(false);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Constructor_0(void* arraylist, void* targ);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Length_0(void* arraylist);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Add_1(void* arraylist, void* elem);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Contains_1(void* arraylist, void* elem);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Get_1(void* arraylist, void* index) noexcept(false);
extern "C" DLLEXPORT void* LIB_NOM_ArrayList_Set_2(void* arraylist, void* index, void* value) noexcept(false);
extern "C" DLLEXPORT void* LIB_NOM_ArrayListEnumerator_Constructor_0(void* ArrayListEnumerator, void* targ, void* arraylist);

