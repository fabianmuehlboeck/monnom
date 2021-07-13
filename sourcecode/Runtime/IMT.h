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
		class NomMethodKey;
		llvm::FunctionType* GetIMTCastFunctionType();
		llvm::FunctionType* GetIMTFunctionType();
		llvm::Function* GenerateIMT(llvm::Module* mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, llvm::SmallVector<std::pair<NomMethodKey*, llvm::Function*>, 8>& imtPairs);
		llvm::Function* GenerateRawInvokeWrap(llvm::Module* mod, llvm::GlobalValue::LinkageTypes linkage, const llvm::Twine name, const NomInterface* ifc, llvm::Function* callCode);


	}
}