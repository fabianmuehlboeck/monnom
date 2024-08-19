#pragma once
#include "NomInterface.h"

namespace Nom
{
	namespace Runtime
	{

		class IComparableInterface : public NomInterfaceInternal
		{
		private:
			IComparableInterface();
			virtual ~IComparableInterface() override;
		public:
			static IComparableInterface* GetInstance();
			virtual void GetClassDependencies(llvm::SmallVector<const NomClassInternal*, 4>& results) const override;
		};


	}
}
