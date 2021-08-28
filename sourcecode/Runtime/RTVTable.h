#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "NomBuilder.h"
#include "RTDescriptor.h"

namespace Nom
{
	namespace Runtime
	{
		llvm::FunctionType* GetMethodEnsureFunctionType();
		//enum class RTVTableFields : unsigned char { InterfaceTable = 0, MethodTable = 1, InterfaceTableOffset = 2, Kind = 3, NomIRLink = 4 };
		enum class RTVTableFields : unsigned char { MethodTable = 0, InterfaceMethodTable = 1, ReturnValueCheckFunction = 2, Kind = 3, NomIRLink = 4 };
		class RTVTable
		{
		public:
			static llvm::StructType* GetLLVMType();

			static llvm::Constant* CreateConstant(RTDescriptorKind kind, const void* irlink, llvm::Constant* interfaceMethodTable, llvm::Constant* checkReturnValueFunction);

			static llvm::Value* GenerateReadKind(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Instruction* GenerateWriteKind(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* kind);
			static llvm::Value* GenerateReadNomIRLink(NomBuilder& builder, llvm::Value* vtablePtr);
			static llvm::Value* GenerateReadInterfaceMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Constant* index);
			static llvm::Value* GenerateReadReturnValueCheckFunction(NomBuilder& builder, llvm::Value* vtablePtr);
			
			static llvm::Value* GenerateReadMethodTableEntry(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value* offset);
			//vtablePtr - pointer to vtable
			//offset - offset of interface table in vtable as specified by offset field
			//index - index of interface entry in interface table
			static llvm::Value* GetInterfaceTableEntryPointer(NomBuilder& builder, llvm::Value* vtablePtr, llvm::Value *offset, llvm::Value* index);

			static void GenerateInitialization(NomBuilder& builder, llvm::Value* vtptr, /*llvm::Value* ifcoffset,*/ llvm::Value* imtptr, llvm::Value* kind, llvm::Value* irdesc);

			//static int GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambdaBlock, llvm::BasicBlock** structBlock, llvm::BasicBlock** partialAppBlock, llvm::BasicBlock** multiCastBlock);
			//static int GenerateVTableKindSwitch(NomBuilder& builder, llvm::Value* vtptr, llvm::BasicBlock** classObjectBlock, llvm::BasicBlock** lambda_targsInObject_Block, llvm::BasicBlock** lambda_targsInVTable_Block, llvm::BasicBlock** struct_targsInObject_Block, llvm::BasicBlock** struct_targsInVTable_Block, llvm::BasicBlock** partialAppBlock,  llvm::BasicBlock** multiCastBlock);
		};
	}
}