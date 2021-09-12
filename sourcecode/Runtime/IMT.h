#pragma once
#include "llvm/IR/Function.h"
#include "llvm/ADT/ArrayRef.h"
#include "Defs.h"
#include "NomInstantiationRef.h"

namespace Nom
{
	namespace Runtime
	{
		class NomInterface;
		class NomMethodTableEntry;
		class NomInterfaceCallTag;
		llvm::FunctionType* GetIMTCastFunctionType();
		llvm::FunctionType* GetFieldReadFunctionType();
		llvm::FunctionType* GetFieldWriteFunctionType();
		llvm::FunctionType* GetIMTFunctionType();

		enum class DynamicDispatchListEntryFields : char { Key = 0, Flags = 1, Dispatcher = 2 };
		llvm::StructType* GetDynamicDispatchListEntryType();
		llvm::Constant* GetDynamicDispatchListEntryConstant(llvm::Constant* key, llvm::Constant* flags, llvm::Constant* dispatcherPtr);
		llvm::StructType* GetDynamicDispatcherLookupResultType();
		llvm::FunctionType* GetCheckReturnValueFunctionType();


		llvm::StructType* GetDynamicDispatcherLookupResultType();
	}
}