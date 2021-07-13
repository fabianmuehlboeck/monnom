#pragma once
#include "NomInterface.h"

namespace Nom
{
	namespace Runtime
	{

		class IEnumerableInterface : public NomInterfaceInternal
		{
		private:
			IEnumerableInterface();
			virtual ~IEnumerableInterface() override;
		public:
			static IEnumerableInterface* GetInstance();
			virtual void GetInterfaceDependencies(llvm::SmallVector<const NomInterfaceInternal*, 4>& results) const override;
		};


	}
}
