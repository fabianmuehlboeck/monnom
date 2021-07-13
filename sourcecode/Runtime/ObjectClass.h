#pragma once
#include "RTClass.h"
#include "NomClass.h"
#include "RTTypes.h"

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

		//class RTObjectClass : public RTClass
		//{
		//public:
		//	RTObjectClass() : RTClass(0, 0)
		//	{
		//	}
		//};
	}
}

//extern const Nom::Runtime::NomObjectClass _NomObjectClass;
//extern const Nom::Runtime::RTObjectClass _RTObjectClass;
//extern const Nom::Runtime::NomClassType * _NomObjectClassTypeRef;
//extern const Nom::Runtime::RTClassType _RTObjectClassType;