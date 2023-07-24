#pragma once
#include "NomClass.h"

namespace Nom
{
	namespace Runtime
	{
		class NomTimerClass : public NomClassInternal
		{
		private:
			NomTimerClass();
		public:
			static NomTimerClass* GetInstance();
			virtual ~NomTimerClass() override;

			virtual size_t GetFieldCount() const override;
		};
	}
}
extern "C" DLLEXPORT void* LIB_NOM_Timer_PrintDifference_1(void* timer) noexcept(false);
extern "C" DLLEXPORT void* LIB_NOM_Timer_Constructor_0(void* timer) noexcept(false);

