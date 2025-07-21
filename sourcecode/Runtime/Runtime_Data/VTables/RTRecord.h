#pragma once
#include "../../Runtime_Data/ARTRep.h"
#include "llvm/IR/DerivedTypes.h"
#include "../../Common/NomBuilder.h"
#include "../../Runtime_Data/VTables/RTPartialApp.h"

namespace Nom
{
	namespace Runtime
	{
		class NomRecord;
		enum class RTStructFields : unsigned char { VTable = 0, FieldCount = 1, TypeArgCount =2 };
		class RTRecord : public ARTRep<RTRecord, RTStructFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Constant* CreateConstant(const NomRecord* structptr, llvm::Function* fieldRead, llvm::Function* fieldWrite, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable);
			static llvm::Value* GenerateReadFieldCount(NomBuilder& builder, llvm::Value* descriptor);
			static llvm::Value* GenerateReadTypeArgCount(NomBuilder& builder, llvm::Value* descriptor);
		};
	}
}