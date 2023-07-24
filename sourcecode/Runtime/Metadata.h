#pragma once
PUSHDIAGSUPPRESSION
#include "llvm/IR/Metadata.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/ArrayRef.h"
POPDIAGSUPPRESSION
namespace Nom
{
	namespace Runtime
	{
		llvm::MDNode* getStructDescriptorInvariantNode();
		llvm::MDNode* getGeneralInvariantNode();
		llvm::MDNode* GetLikelyFirstBranchMetadata();
		llvm::MDNode* GetLikelySecondBranchMetadata();
		llvm::MDNode* GetBranchWeightsForBlocks(llvm::ArrayRef<llvm::BasicBlock*> blocks);
		llvm::MDNode* GetBranchWeights(llvm::ArrayRef<uint64_t> weights);
	}
}
