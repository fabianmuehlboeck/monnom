#include "CondBranchInstruction.h"
#include "PhiNode.h"
#include "../TypeOperations.h"
#include <iostream>
#include <tuple>
#include "../NomValue.h"
#include "../CompileEnv.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		CondBranchInstruction::CondBranchInstruction(RegIndex _condition, size_t _thenTarget, size_t _elseTarget) : NomInstruction(OpCode::CondBranch), Condition(_condition), ThenTarget(_thenTarget), ElseTarget(_elseTarget)
		{
		}


		CondBranchInstruction::~CondBranchInstruction()
		{
		}
		void CondBranchInstruction::Compile(NomBuilder& builder, CompileEnv* env, [[maybe_unused]] size_t lineno)
		{
			PhiNode *thenPhi = env->GetPhiNode(ThenTarget);
			PhiNode *elsePhi = env->GetPhiNode(ElseTarget);
			RTValuePtr condition = (*env)[Condition]->AsRawBool(builder);
			
			auto thenIncCount = ThenIncomings.size();
			auto elseIncCount = ElseIncomings.size();
			auto incCount = thenIncCount + elseIncCount;
			using incPair = std::pair< llvm::PHINode*, llvm::Value*>;
			auto incPairs = makealloca(incPair, incCount);
			decltype(incCount) incPos = 0;

			for (auto ti : ThenIncomings)
			{
				RTValuePtr nv = (*env)[std::get<0>(ti)];
				llvm::PHINode* llvmPHI = static_cast<llvm::PHINode*>(static_cast<llvm::Value*>(nv));
				auto incval = (*env)[std::get<1>(ti)]->ForLLVMType(builder, llvmPHI->getType(), false);
				incPairs[incPos] = std::make_pair(llvmPHI, incval);
				incPos++;
			}
			for (auto ei : ElseIncomings)
			{
				RTValuePtr nv = (*env)[std::get<0>(ei)];
				llvm::PHINode* llvmPHI = static_cast<llvm::PHINode*>(static_cast<llvm::Value*>(nv));
				auto incval = (*env)[std::get<1>(ei)]->ForLLVMType(builder, llvmPHI->getType(), false);
				incPairs[incPos] = std::make_pair(llvmPHI, incval);
				incPos++;
			}
			BasicBlock* curBlock = builder->GetInsertBlock();
			while (incPos > 0)
			{
				incPos--;
				incPairs[incPos].first->addIncoming(incPairs[incPos].second, curBlock);
			}
			builder->CreateCondBr(condition, thenPhi->getBlock(), elsePhi->getBlock());
			env->basicBlockTerminated = true;
		}
		void CondBranchInstruction::Print([[maybe_unused]] bool resolve)
		{
			cout << "CondBranch #" << std::dec << Condition;
			cout << ", " << std::dec << ThenTarget;
			cout << ", " << std::dec << ElseTarget;
			cout << "\n";
		}
		void CondBranchInstruction::FillConstantDependencies([[maybe_unused]] NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}
