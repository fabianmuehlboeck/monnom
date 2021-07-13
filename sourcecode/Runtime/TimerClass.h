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