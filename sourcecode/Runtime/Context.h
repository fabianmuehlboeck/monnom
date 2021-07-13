#pragma once

#include "llvm/IR/LLVMContext.h"

namespace Nom
{
	namespace Runtime
	{
		::llvm::LLVMContext &TheContext();
	}
}

#define LLVMCONTEXT (::Nom::Runtime::TheContext())