#pragma once

#include "PWRefValue.h"
#include "NomValue.h"
#include <inttypes.h>
#include "PWTypeArr.h"
#include "PWInt.h"

namespace Nom
{
	namespace Runtime
	{
		class PWType;
		class PWObject : public PWRefValue
		{
		public:
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWObject(llvm::Value* _wrapped) : PWRefValue(_wrapped)
			{

			}

			llvm::Value* ReadField(NomBuilder &builder, PWInt32 index, bool targetHasRawInvoke);
			void WriteField(NomBuilder &builder, PWInt32 index, llvm::Value* value, bool targetHasRawInvoke);
			PWTypeArr PointerToTypeArguments(NomBuilder& builder);
			PWType ReadTypeArgument(NomBuilder& builder, PWInt32 index);
			void WriteTypeArgument(NomBuilder& builder, PWInt32 index, PWType tp);
		};
	}
}
