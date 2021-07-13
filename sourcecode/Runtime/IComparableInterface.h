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
		};


	}
}
