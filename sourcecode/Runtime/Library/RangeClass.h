#pragma once
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomClass.h"
#include "../Runtime/NomVMInterface.h"

namespace Nom
{
	namespace Runtime
	{
		class ObjectHeader;
		class RangeClass : public NomClassInternal
		{
		private:
			RangeClass();
		public:
			static RangeClass* GetInstance();
			virtual ~RangeClass() override {}
			virtual size_t GetFieldCount() const override;
			virtual void GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const override;
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;
		};

		class RangeEnumeratorClass : public NomClassInternal
		{
		private:
			RangeEnumeratorClass();
		public:
			static RangeEnumeratorClass* GetInstance();
			virtual ~RangeEnumeratorClass() override {}
			virtual size_t GetFieldCount() const override;
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;

		};
	}
}