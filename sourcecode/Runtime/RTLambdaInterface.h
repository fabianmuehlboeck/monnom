#pragma once
#include "ARTRep.h"
#include "RTDescriptor.h"
#include "NomBuilder.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTLambdaInterfaceFields : unsigned char { TypeArgs = 0, RTInterface = 1, OrigInterface = 2, RawInvokeWrapper = 3 };
		class RTLambdaInterface : public ARTRep<RTLambdaInterface, RTLambdaInterfaceFields>
		{
		public:
			static llvm::StructType* GetLLVMType();
			static void GenerateWriteMethodPointer(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* methodPtr, llvm::Value* index);
			static llvm::Value* GetTypeArgumentsPointer(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GetTypeArgumentNegativeIndex(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* index);
			static llvm::Constant* CreateGlobalConstant(llvm::Module& mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, RTDescriptorKind kind, const NomInterface* irptr, llvm::Constant* typeArgCount, llvm::Constant* superTypesCount, llvm::Constant* superTypeEntries, llvm::Constant* origInterfacePointer, llvm::Constant* instantiationDictionary, llvm::Function* functionptr, bool optimizedTypeVarAccess);
			static llvm::Value* CreateCopyVTable(NomBuilder& builder, llvm::Value* vtable, llvm::Value* typeArgCount);
			static llvm::Value* GetRawInvokeWrapper(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateReadOrigInterface(NomBuilder& builder, llvm::Value* vtablePtr);
		};
	}
}