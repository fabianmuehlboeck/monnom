#pragma once
#include "PWrapper.h"
#include "RefValueHeader.h"
#include "NomBuilder.h"
#include "PWVTable.h"

namespace Nom
{
	namespace Runtime
	{
		class PWRefValue : public PWrapper
		{
		public:
			PWRefValue(llvm::Value* wrapped) : PWrapper(wrapped)
			{

			}
			llvm::Value* ReadTypeTag(NomBuilder& builder) const;
			PWVTable ReadVTable(NomBuilder& builder) const;
			operator NomValue() const
			{
				return wrapped;
			}
			NomValue AsNomValue() const
			{
				return wrapped;
			}
		};
	}
}