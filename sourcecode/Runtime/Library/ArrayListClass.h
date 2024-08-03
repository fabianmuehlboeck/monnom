#pragma once
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomClass.h"
#include "../Runtime/NomVMInterface.h"

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