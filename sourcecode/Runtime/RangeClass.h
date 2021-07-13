#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "NomVMInterface.h"

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