#include "BranchInstruction.h"
#include "PhiNode.h"
#include "../TypeOperations.h"
#include <iostream>

using namespace std;
namespace Nom
{
	namespace Runtime
	{

		BranchInstruction::~BranchInstruction()
		{
		}
		void BranchInstruction::Compile(NomBuilder& builder, CompileEnv* env, int lineno)
		{
			PhiNode *phi = env->GetPhiNode(Target);
			auto incCount = Incomings.size();
			using incPair = std::pair< llvm::PHINode*, llvm::Value*>;
			auto incPairs = makealloca(incPair, incCount);
			decltype(incCount) incPos = 0;
			for (auto &i : Incomings)
			{
				NomValue nv = (*env)[std::get<0>(i)];
				llvm::PHINode* llvmPHI = ((llvm::PHINode*)(llvm::Value*)nv);
				auto incVal = EnsureType(builder, env, (*env)[std::get<1>(i)], nv.GetNomType(), llvmPHI->getType());
				incPairs[incPos] = std::make_pair(llvmPHI, incVal);
				incPos++;
			}
			auto incBlock = builder->GetInsertBlock();
			while (incPos > 0)
			{
				incPos--;
				incPairs[incPos].first->addIncoming(incPairs[incPos].second, incBlock);
			}
			builder->CreateBr(phi->getBlock());
			env->basicBlockTerminated = true;
		}
		void BranchInstruction::Print(bool resolve)
		{
			cout << "Branch %" << std::dec << Target << " {";
			bool first = true;
			for (auto &i : Incomings)
			{
				if (!first)
				{
					cout << ", ";
				}
				first = false;
				cout << "[" << std::dec << std::get<0>(i) << "; " << std::get<1>(i) << "]";
			}
			cout << "}\n";
		}
		void BranchInstruction::FillConstantDependencies(NOM_CONSTANT_DEPENCENCY_CONTAINER& result)
		{
		}
	}
}