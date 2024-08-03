#pragma once
#include "../Intermediate_Representation/Data/NomInterface.h"

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