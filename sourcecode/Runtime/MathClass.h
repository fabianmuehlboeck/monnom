#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "RTTypes.h"

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