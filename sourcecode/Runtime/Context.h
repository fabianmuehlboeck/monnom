#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/LLVMContext.h"
POPDIAGSUPPRESSION
namespace Nom
{
	namespace Runtime
	{
		::llvm::LLVMContext &TheContext();
	}
}

#define LLVMCONTEXT (::Nom::Runtime::TheContext())
