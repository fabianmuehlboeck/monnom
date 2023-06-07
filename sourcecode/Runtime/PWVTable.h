#pragma once
#include "PWrapper.h"
#include "RTVTable.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		class PWVTable : public PWrapper
		{
		public:
			PWVTable(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			llvm::Value* ReadHasRawInvoke(NomBuilder& builder) const;
			llvm::Value* ReadMethodPointer(NomBuilder& builder, llvm::Constant* index) const;
		};
	}
}