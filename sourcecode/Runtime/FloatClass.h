#pragma once
#include "NomClass.h"

namespace Nom
{
	namespace Runtime
	{
		class NomFloatClass : public NomClassInternal
		{
		private:
			NomFloatClass();
		public:
			static NomFloatClass *GetInstance();
			virtual ~NomFloatClass() override;
		};

		//class RTFloatClass : public RTClass
		//{
		//public:
		//	RTFloatClass();
		//	~RTFloatClass() {}
		//};
	}
}


////extern const Nom::Runtime::RTFloatClass _RTFloatClass;
//extern const Nom::Runtime::NomFloatClass _NomFloatClass;
//extern const Nom::Runtime::NomFloatClass * const _NomFloatClassRef;
////extern const Nom::Runtime::RTFloatClass * const _RTFloatClassRef;
//extern const Nom::Runtime::NomClass * const _NomFloatClassNC;
////extern const Nom::Runtime::RTClass * const _RTFloatClassRTC;