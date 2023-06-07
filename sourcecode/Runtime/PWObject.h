#pragma once

#include "PWRefValue.h"
#include "NomValue.h"
#include <inttypes.h>

namespace Nom
{
	namespace Runtime
	{
		class PWTypeArr;
		class PWType;
		class PWObject : public PWRefValue
		{
		public:
			static llvm::Type* GetLLVMType();
			PWObject(llvm::Value* wrapped) : PWRefValue(wrapped)
			{

			}

			llvm::Value* ReadField(NomBuilder &builder, int index, bool targetHasRawInvoke);
			llvm::Value* ReadField(NomBuilder &builder, llvm::Value* index, bool targetHasRawInvoke);
			void WriteField(NomBuilder &builder, int index, llvm::Value* value, bool targetHasRawInvoke);
			void WriteField(NomBuilder &builder, llvm::Value* index, llvm::Value* value, bool targetHasRawInvoke);
			PWTypeArr PointerToTypeArguments(NomBuilder& builder);
			PWType ReadTypeArgument(NomBuilder& builder, int32_t index);
			PWType ReadTypeArgument(NomBuilder& builder, llvm::Value* index);
			void WriteTypeArgument(NomBuilder& builder, int32_t index, PWType tp);
			void WriteTypeArgument(NomBuilder& builder, llvm::Value* index, PWType tp);
		};
	}
}