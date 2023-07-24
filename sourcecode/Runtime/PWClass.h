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
			static llvm::Type* GetLLVMType();
			static llvm::Type* GetWrappedLLVMType();
			PWClass(llvm::Value* _wrapped);
			static PWClass FromVTable(PWVTable vt);

			PWInterface GetInterface(NomBuilder& builder) const;
			PWInt64 ReadFieldCount(NomBuilder& builder) const;
		};
	}
}
