#pragma once
#include "ARTRep.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTStructInterfaceFields :unsigned char { TypeArgs = 0, RTInterface = 1, OrigInterface = 2  };
		class RTStructInterface : public ARTRep<RTStructInterface, RTStructInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static llvm::Value* GetTypeArgumentsPointer(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GetTypeArgumentNegativeIndex(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* index);
			static llvm::Value* GetOriginalInterface(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* origInterfacePointer, llvm::Constant* instantiationDictionary, bool optimizedTypeVarAccess);
			static llvm::Value* CreateCopyVTable(NomBuilder& builder, llvm::Value* vtable/*, llvm::Value* typeArgCount*/);
		};
	}
}