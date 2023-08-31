#pragma once
#include "PWrapper.h"
#include "RefValueHeader.h"
#include "NomBuilder.h"
#include "PWVTable.h"

namespace Nom
{
	namespace Runtime
	{
		class PWIMTFunction;
		class PWRefValue : public PWrapper
		{
		public:
			PWRefValue(llvm::Value* _wrapped) : PWrapper(_wrapped)
			{

			}
			PWVTable ReadVTable(NomBuilder& builder) const;
			void WriteVTable(NomBuilder& builder, PWVTable vtbl) const;
			llvm::Value* WriteVTableCMPXCHG(NomBuilder& builder, PWVTable vtbl, PWVTable orig) const;
			operator NomValue() const
			{
				return wrapped;
			}
			NomValue AsNomValue() const
			{
				return wrapped;
			}
			llvm::Value* ReadRawInvoke(NomBuilder& builder) const;
			void WriteRawInvoke(NomBuilder& builder, llvm::Value* rawinvoke) const;
			llvm::AtomicCmpXchgInst* WriteRawInvokeCMPXCHG(NomBuilder& builder, llvm::Value* rawinvoke, llvm::Value* orig) const;
			PWIMTFunction GetIMTFunction(NomBuilder& builder, PWCInt32 idx, size_t lineno) const;
		};
	}
}
