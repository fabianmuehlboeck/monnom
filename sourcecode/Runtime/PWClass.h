#pragma once
#include "PWrapper.h"
#include "PWVTable.h"
#include "PWInterface.h"
#include "RTClass.h"

namespace Nom
{
	namespace Runtime
	{
		class PWClass : public PWVTable
		{
		public:
			PWClass(llvm::Value* wrapped) : PWVTable(wrapped)
			{

			}
			static PWClass FromVTable(PWVTable vt);

			PWInterface GetInterface(NomBuilder& builder) const;
		};
	}
}