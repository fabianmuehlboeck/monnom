#pragma once
#include "NomInterface.h"
#include "NomTypeParameter.h"

namespace Nom
{
	namespace Runtime
	{

		class FunInterface : public NomInterfaceInternal
		{
		private:
			FunInterface(std::string &name);
			virtual ~FunInterface() override;
		public:
			static FunInterface* GetFun(size_t argcount);
		};


	}
}
