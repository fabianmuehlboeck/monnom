#include "CondBranchInstruction.h"
#include "PhiNode.h"
#include "../TypeOperations.h"
#include <iostream>
#include <tuple>
#include "../NomValue.h"

using namespace llvm;
using namespace std;
namespace Nom
{
	namespace Runtime
	{

		CondBranchInstruction::CondBranchInstruction(RegIndex condition, int thenTarget, int elseTarget) : NomInstruction(OpCode::CondBranch), Condition(condition), ThenTarget(thenTarget), ElseTarget(elseTarget)
		{
		}


		CondBranchInstruction::~CondBranchInstruction()
		{
		}
		void CondBranchInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			PhiNode *thenPhi = env->GetPhiNode(ThenTarget);
			PhiNode *elsePhi = env->GetPhiNode(ElseTarget);
			auto condition = EnsureUnpackedBool(builder, env, (*env)[Condition]);
			
			auto thenIncCount = ThenIncomings.size();
			auto elseIncCount = ElseIncomings.size();
			auto incCount = thenIncCount + elseIncCount;
			using incPair = std::pair< llvm::PHINode*, llvm::Value*>;
			auto incPairs = makealloca(incPair, incCount);
			decltype(incCount) incPos = 0;

			for (auto ti : ThenIncomings)
			{
				NomValue nv = (*env)[std::get<0>(ti)];
				llvm::PHINode* llvmPHI = ((llvm::PHINode*)(llvm::Value*)nv);
				auto incval = EnsureType(builder, env, (*env)[std::get<1>(ti)], nv.GetNomType(), llvmPHI->getType());
				incPairs[incPos] = std::make_pair(llvmPHI, incval);
				incPos++;
				//((llvm::PHINode*)(llvm::Value*)nv)->addIncoming(incval, builder->GetInsertBlock());
			}
			for (auto ei : ElseIncomings)
			{
				NomValue nv = (*env)[std::get<0>(ei)];
				llvm::PHINode* llvmPHI = ((llvm::PHINode*)(llvm::Value*)nv);
				auto incval = EnsureType(builder, env, (*env)[std::get<1>(ei)], nv.GetNomType(), llvmPHI->getType());
				incPairs[incPos] = std::make_pair(llvmPHI, incval);
				incPos++;
				//((llvm::PHINode*)(llvm::Value*)nv)->addIncoming(incval, builder->GetInsertBlock());
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
		void CondBranchInstruction::Print(bool resolve)
		{
			cout << "CondBranch #" << std::dec << Condition;
			cout << ", " << std::dec << ThenTarget;
			cout << ", " << std::dec << ElseTarget;
			cout << "\n";
		}
		void CondBranchInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}