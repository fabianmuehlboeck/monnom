#pragma once
#include "../Intermediate_Representation/Data/NomInterface.h"

namespace Nom
{
	namespace Runtime
	{

		class IPairInterface : public NomInterfaceInternal
		{
		private:
			IPairInterface();
			virtual ~IPairInterface() override;
		public:
			static IPairInterface* GetInstance();
		};


	}
}
#pragma once
