#pragma once
#include "NomInterface.h"

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
