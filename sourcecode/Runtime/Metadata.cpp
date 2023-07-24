#include "Metadata.h"
#include "Context.h"
#include "CompileHelpers.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		MDNode* getStructDescriptorInvariantNode()
		{
			return getGeneralInvariantNode();
		}
		llvm::MDNode* getGeneralInvariantNode()
		{
			static MDNode* mdn = MDNode::get(LLVMCONTEXT, { });
			return mdn;
		}
		llvm::MDNode* GetLikelyFirstBranchMetadata()
		{
			static MDNode* mdn = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"), ConstantAsMetadata::get(MakeInt32(9)), ConstantAsMetadata::get(MakeInt32(1)) });
			return mdn;
		}
		llvm::MDNode* GetLikelySecondBranchMetadata()
		{
			static MDNode* mdn = MDNode::get(LLVMCONTEXT, { MDString::get(LLVMCONTEXT, "branch_weights"), ConstantAsMetadata::get(MakeInt32(1)), ConstantAsMetadata::get(MakeInt32(9)) });
			return mdn;
		}
		llvm::MDNode* GetBranchWeightsForBlocks(llvm::ArrayRef<llvm::BasicBlock*> blocks)
		{
			Metadata** args = makealloca(Metadata*, blocks.size() + 1);
			args[0] = MDString::get(LLVMCONTEXT, "branch_weights");
			for (size_t i = 0; i < blocks.size(); i++)
			{
				size_t weight = 10;
				BasicBlock* curBlock = blocks[i];
				BasicBlock* checkBlock = curBlock;
				int depth = 0;
				while (checkBlock->getTerminator() != nullptr && depth < 20)
				{
					depth++;
					auto terminator = checkBlock->getTerminator();
					if (terminator->getOpcode() == llvm::Instruction::TermOps::Unreachable)
					{
						weight = 0;
						break;
					}
					if (terminator->getOpcode() == llvm::Instruction::TermOps::Br)
					{
						BranchInst* br = static_cast<BranchInst*>(terminator);
						if (br->isUnconditional())
						{
							checkBlock = br->getSuccessor(0);
							continue;
						}
					}
					break;
				}
				args[i + 1] = ConstantAsMetadata::get(MakeUInt(64, weight));
			}
			return MDNode::get(LLVMCONTEXT, ArrayRef<Metadata*>(args, blocks.size() + 1));
		}
		llvm::MDNode* GetBranchWeights(llvm::ArrayRef<uint64_t> weights)
		{
			Metadata** args = makealloca(Metadata*, weights.size() + 1);
			args[0] = MDString::get(LLVMCONTEXT, "branch_weights");
			for (size_t i = 0; i < weights.size(); i++)
			{
				args[i + 1] = ConstantAsMetadata::get(MakeUInt(64, weights[i]));
			}
			return MDNode::get(LLVMCONTEXT, ArrayRef<Metadata*>(args, weights.size() + 1));
		}
	}
}
