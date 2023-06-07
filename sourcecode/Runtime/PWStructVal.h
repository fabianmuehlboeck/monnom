#pragma once
#include <inttypes.h>
#include "PWrapper.h"
#include "PWRefValue.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class PWTypeArr;
		class PWCastData;
		class PWStructVal : public PWRefValue
		{
		public:
			PWStructVal(llvm::Value* wrapped) : PWRefValue(wrapped)
			{

			}

			llvm::Value* ReadTypeArgument(NomBuilder& builder, int32_t index);
			llvm::Value* ReadTypeArgument(NomBuilder& builder, llvm::Value* index);
			PWTypeArr PointerToTypeArgs(NomBuilder& builder);

			PWCastData ReadCastData(NomBuilder& builder);
			llvm::Value* WriteCastDataCMPXCHG(NomBuilder& builder, PWCastData olddata, PWCastData newdata);
		};
	}
}