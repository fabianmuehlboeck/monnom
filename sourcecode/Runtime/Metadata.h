#pragma once
#include "llvm/IR/Metadata.h"
namespace Nom
{
	namespace Runtime
	{
		llvm::MDNode* getStructDescriptorInvariantNode();
		llvm::MDNode* getGeneralInvariantNode();
	}
}