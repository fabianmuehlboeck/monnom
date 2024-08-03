#pragma once
#include "../Runtime_Data/VTables/RTClass.h"
#include "../Intermediate_Representation/Data/NomClass.h"

namespace Nom
{
	namespace Runtime
	{
		class NomClassType;
		class NomMathClass : public NomClassInternal
		{
		private:
			NomMathClass();
		public:
			static NomMathClass* GetInstance();
			virtual ~NomMathClass() override {}
		};
	}
}