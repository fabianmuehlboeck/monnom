#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Function.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/GlobalValue.h"
POPDIAGSUPPRESSION
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

		llvm::Function* GenerateCheckReturnTypesFunction(llvm::Module* mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, llvm::SmallVector<std::tuple<NomInterfaceCallTag*, llvm::Function*, NomType*>, 8>& imtPairs);


		llvm::StructType* GetDynamicDispatcherLookupResultType();
	}
}
