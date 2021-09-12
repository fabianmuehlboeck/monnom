#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Value.h"
#include "NomBuilder.h"
#include "RTDescriptor.h"

namespace Nom
{
	namespace Runtime
	{
		enum class RTVTableFields : unsigned char { MethodTable = 0, Kind = 1, InterfaceMethodTable = 2, DynamicDispatcherTable = 3, ReadField = 4, WriteField = 5 };
		class RTVTable
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Constant* CreateConstant(RTDescriptorKind kind, llvm::Constant* interfaceMethodTable, llvm::Constant* dynamicDispatcherTable, llvm::Constant* fieldLookupFunction, llvm::Constant* fieldStoreFunction);

			static llvm::Value* GenerateReadKind(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateReadInterfaceMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Constant* index);
			static llvm::Value* GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset);
			static llvm::Value* GenerateReadReadFieldFunction(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateReadWriteFieldFunction(NomBuilder& builder, llvm::Value* vtablePtr);

			static llvm::Value* GenerateHasRawInvoke(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateIsNominalValue(NomBuilder& builder, llvm::Value* vtablePtr);

			static void GenerateFreezeMethodField(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtablePtr, llvm::Value* name, llvm::Value* tableIndex);
			static llvm::Value* GenerateFindDynamicDispatcherPair(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtablePtr, size_t name);
			static llvm::Value* GenerateFindDynamicDispatcherPair(NomBuilder& builder, llvm::Value* refValue, llvm::Value* vtablePtr, llvm::Value* name, llvm::Value* tableIndex);

		};
	}
}