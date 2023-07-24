#pragma once
#include "RTClass.h"
#include "NomClass.h"

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

extern "C" DLLEXPORT double LIB_NOM_Math_Sin_1(double number);
extern "C" DLLEXPORT double LIB_NOM_Math_Cos_1(double number);
extern "C" DLLEXPORT double LIB_NOM_Math_Tan_1(double number);
extern "C" DLLEXPORT double LIB_NOM_Math_Sqrt_1(double number);
extern "C" DLLEXPORT double LIB_NOM_Math_FMax_2(double left, double right);
extern "C" DLLEXPORT double LIB_NOM_Math_FMin_2(double left, double right);
