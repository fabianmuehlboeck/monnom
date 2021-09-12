#pragma once
#include "RTClass.h"
#include "NomClass.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClassType;
		class NomObjectClass : public NomClassInternal
		{
		private:
			NomObjectClass();
		public:
			static NomObjectClass *GetInstance();
			virtual ~NomObjectClass() override {}
		};

	}
}