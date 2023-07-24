#pragma once
#include "RTClass.h"
#include "NomClass.h"

extern "C" DLLEXPORT void* LIB_NOM_Object_Constructor_0(void* obj);

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
