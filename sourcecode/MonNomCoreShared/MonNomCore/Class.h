#pragma once
#include "Interface.h"

namespace Nom
{
	namespace Runtime
	{

		extern "C" class MonNomClass : MonNomInterface
		{
		private:
			void* name = nullptr;
			void** methodtable = nullptr;
			const int argcount = 0;
			const int fieldcount = 0;

			MonNomClass();
			~MonNomClass();
		public:
			static MonNomClass* MakeClass()
			{
				return new MonNomClass();
			}
		};
	}
}

extern "C" Nom::Runtime::MonNomClass * NOMRT_MakeRTClass()
{
	return Nom::Runtime::MonNomClass::MakeClass();
}
