#pragma once
#include "NomInterface.h"

namespace Nom
{
	namespace Runtime
	{

		class IEnumeratorInterface : public NomInterfaceInternal
		{
		private:
			IEnumeratorInterface();
			virtual ~IEnumeratorInterface() override;
		public:
			static IEnumeratorInterface* GetInstance();
		};


	}
}
