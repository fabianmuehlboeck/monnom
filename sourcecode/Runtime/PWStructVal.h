#pragma once
#include <inttypes.h>
#include "PWrapper.h"
#include "PWRefValue.h"
#include "NomBuilder.h"
#include "PWTypeArr.h"

namespace Nom
{
	namespace Runtime
	{
		class PWCastData;
		class PWStructVal : public PWRefValue
		{
		public:
			PWStructVal(llvm::Value* _wrapped) : PWRefValue(_wrapped)
			{

			}

			llvm::Value* ReadTypeArgument(NomBuilder& builder, size_t index);
			llvm::Value* ReadTypeArgument(NomBuilder& builder, llvm::Value* index);
			PWTypeArr PointerToTypeArguments(NomBuilder& builder);

			PWCastData ReadCastData(NomBuilder& builder);
			llvm::Value* WriteCastDataCMPXCHG(NomBuilder& builder, PWCastData olddata, PWCastData newdata);
		};
	}
}
