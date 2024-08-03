#pragma once
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomClass.h"

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